#include "StdAfx.h"
#include "InstallerBase.h"
#include "InstanceEnumerator.h"
#include "NonPnpDevnode.h"
#include "ServiceControlManager.h"
#include "SetupInfFile.h"
#include "SystemInfoClass.h"
#include <vector>
#include <iostream>

using namespace std;

CInstallerBase::CInstallerBase(const wchar_t* pInfPath):
	m_infPath(pInfPath ? pInfPath : L"HidEmulator.inf"),
	m_bMustCopy(false)
{
	// Convert the INF path to an absolute path, if needed:
	if(PathIsRelative(m_infPath.c_str()))
	{
		wchar_t relToWhat[MAX_PATH + 1];
		if(!GetModuleFileNameW(nullptr, relToWhat, NUMBER_OF(relToWhat)))
			// Assume that we must copy.
		{
			m_bMustCopy = true;
			return;
		}

		// Extract the name portion of the executable path:
		wchar_t* pName = PathFindFileNameW(relToWhat);
		
		// Kill the filename part if it is set:
		size_t len;
		if(pName != relToWhat)
			len = pName - relToWhat;
		else
			// Otherwise, use the CWD as the path name:
		{
			// Get the current working directory
			GetCurrentDirectory(MAX_PATH, relToWhat);

			// Get the length of the working directory
			len = wcslen(relToWhat);

			// Add a trailing backslash
			relToWhat[len++] = '\\';
		}

		// Null-terminate
		relToWhat[len] = 0;

		// Prepend the inf path with the relative location:
		m_infPath.insert(m_infPath.begin(), relToWhat, relToWhat + len);
	}

	// Determine if the supplied path is on a network drive.  PNP cannot install drivers
	// from networked locations, so copying to a local device becomes necessary.
	m_bMustCopy = !!PathIsNetworkPath(m_infPath.c_str());
}

CInstallerBase::~CInstallerBase(void)
{
}

void CInstallerBase::Install(void)
{
  // Obtain a current list of all known INFs:
  vector<wchar_t> infFiles(0x8000);
  DWORD reqSize = (DWORD)infFiles.size();

  if(!SetupGetInfFileList(nullptr, INF_STYLE_WIN4, &infFiles[0], reqSize, &reqSize)) {
    if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
      throw eHidInstInfCannotEnumerate;

    // Insufficient buffer, size up
    infFiles.resize(reqSize);
    SetupGetInfFileList(nullptr, INF_STYLE_WIN4, &infFiles[0], reqSize, &reqSize);
  }

  // Load our own INF file:
  SetupInfFile leapInf(m_infPath.c_str());
  if(!leapInf.IsLeap())
    throw eHidInstInfApparentlyNotLeap;

  // Latest-known version:
  auto& ver = leapInf.GetVersion();

  // Enumerate INFs, scanning for older versions of ourself:
  for(wchar_t* pCur = &infFiles[0]; *pCur; pCur += wcslen(pCur) + 1) {
    SetupInfFile curInfFile(pCur);
    if(!curInfFile.IsLeap())
      continue;

    // Version comparison:
    if(ver < curInfFile.GetVersion())
    {
      // Uninstall this one.
      SetupUninstallOEMInf(pCur, 0, nullptr);
    }
  }

	// The SYSTEM device class is where the device will be installed
	std::shared_ptr<SystemInfoClass> hInfo(new SystemInfoClass);

  NonPnpDevnode devInfo;
  {
    InstanceEnumerator ie;
    if(ie.Next())
      // Just use the already-existing devnode
      devInfo = NonPnpDevnode(ie, ie.Current());
    else
	    // We next create an empty devnode where the ocuhid legacy device may be attached.
	    // This empty devnode will then be characterized with a PNPID (by us) and then we let PNP
	    // find and load the driver from there.  This is basically what the add/remove hardware wizard
	    // does when you add legacy hardware.
      devInfo = NonPnpDevnode(hInfo);

    // Destory any other detected device--ensure a maximum of one is ever installed:
    while(ie.Next())
      ie.DestroyCurrent();
    if(ie.IsRestartRequired())
      RequireRestart();
  }
  
	// Copy the new INF to its destination in the system:
	if(!SetupCopyOEMInf(m_infPath.c_str(), nullptr, SPOST_NONE, 0, nullptr, 0, nullptr, nullptr))
		throw
			PathFileExists(m_infPath.c_str()) ?
			eHidInstINFDependencyMissing :
			eHidInstCopyOEMFail;

  // Associate the new driver with the PNP devnode:
  devInfo.Associate();

  // Now we'll select the device:
  if(devInfo.InstallDriver())
    RequireRestart();
}

void CInstallerBase::Update(void)
{
  InstanceEnumerator ie;
  if(!ie.Next())
    throw eHidInstNoDevsToUpdate;

	// This is an omnibus routine that will update a device if you provide its PNP ID
	// Though we don't strictly need to put this in a ForEach call, it is done anyway
	// to ensure that the routine isn't called when there isn't a need for it.
  BOOL bReboot;
	if(!UpdateDriverForPlugAndPlayDevices(
			nullptr,
			gc_pnpID,
			m_infPath.c_str(),
			0,
			&bReboot
		)
	)
		throw eHidInstUserCancel;

  // Destroy all other detected devices:
  while(ie.Next())
    ie.DestroyCurrent();
  
  // Update o restart disposition
  if(bReboot || ie.IsRestartRequired())
    RequireRestart();
}

void CInstallerBase::Uninstall(void)
{
  // Removal of all detected devices:
  {
    InstanceEnumerator ie;
    while(ie.Next())
      ie.DestroyCurrent();
    if(ie.IsRestartRequired())
      RequireRestart();
  }

  {
	  // Service destruction:
    ServiceControlManager scm;

	  // Now the services proper may be deleted:
	  scm.DeleteOcuHidService(L"HidEmulator");
	  scm.DeleteOcuHidService(L"HidEmulatorKmdf");

    // Propagate the restart flag out:
    if(scm.IsRestartRequired())
      RequireRestart();
  }
}
