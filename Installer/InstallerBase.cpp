#include "StdAfx.h"
#include "InstallerBase.h"
#include "DriverPackage.h"
#include "InstanceEnumerator.h"
#include "NonPnpDevnode.h"
#include "ServiceControlManager.h"
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
  // Destroy all existing devices.  This triggers an unload of our driver and enables us to tinker
  // safely with the device configuration.
  {
    InstanceEnumerator ie;
    while(ie.Next())
      ie.DestroyCurrent();

    if(ie.IsRestartRequired())
      RequireRestart();
  }
  
	// The SYSTEM device class is where the device will be installed
	std::shared_ptr<SystemInfoClass> hInfo(new SystemInfoClass);

	// We next create an empty devnode where the ocuhid legacy device may be attached.
	// This empty devnode will then be characterized with a PNPID (by us) and then we let PNP
	// find and load the driver from there.  This is basically what the add/remove hardware wizard
	// does when you add legacy hardware.
  NonPnpDevnode devNode(hInfo);

  // Perform the actual installation:
  BOOL needReboot;
  DWORD rs = DoInstallPackage(m_infPath.c_str(), needReboot);
  switch(rs)
  {
  case ERROR_NO_SUCH_DEVINST:
    break;
  default:
    if(FAILED(rs))
    {
      SetLastError(rs);
      throw eHidInstDriverPackageRejected;
    }
  }

  // Now we can release the devnode:
  devNode.Release();

  // Now we'll select the device:
  if(needReboot)
    RequireRestart();
}

void CInstallerBase::Update(void)
{
  InstanceEnumerator ie;
  if(!ie.Next())
    throw eHidInstNoDevsToUpdate;
  
  // Now we just perform an installation:
  Install();
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

	  // Now the services proper may be deleted--we don't care if this operation fails:
    try {
	    scm.DeleteOcuHidService(L"HidEmulator");
    } catch(...) {}

    try {
	    scm.DeleteOcuHidService(L"HidEmulatorKmdf");
    } catch(...) {}

    // Propagate the restart flag out:
    if(scm.IsRestartRequired())
      RequireRestart();
  }

  // Package uninstallation:
  BOOL needReboot;
  DWORD rs = DoUninstallPackage(m_infPath.c_str(), needReboot);
  if(FAILED(rs))
  {
    SetLastError(rs);
    throw eHidInstFailedToRemovePackage;
  }

  if(needReboot)
    RequireRestart();
}
