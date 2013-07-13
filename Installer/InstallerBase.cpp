#include "StdAfx.h"
#include "InstallerBase.h"
#include "NonPnpDevnode.h"
#include "SystemInfoClass.h"
#include <vector>
#include <iostream>

using namespace std;

CInstallerBase::CInstallerBase(const wchar_t* pInfPath):
	hInfo(nullptr),
	hMngr(nullptr),
  m_restartRequired(false),
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
	// Clean up the info list and service manager handles, if they are non-null
	if(hInfo)
		SetupDiDestroyDeviceInfoList(hInfo);
	if(hMngr)
		CloseServiceHandle(hMngr);
}

eHidStatus CInstallerBase::Init(void)
{
#if OCU_USE_UPDATEDRIVER
	// The UpdateDriver routine cannot support an install operation from a network path.
	// Thus, if the bMustCopy flag is cleared, we don't allow an installation to proceed.
	if(m_bMustCopy)
		return eHidInstUpdateFailFromNetwork;
#endif

	return eHidInstSuccess;
}

void CInstallerBase::Install(void)
{
	// Copy the INF to its destination in the system:
#if !OCU_USE_UPDATEDRIVER
	if(!SetupCopyOEMInf(m_infPath.c_str(), nullptr, SPOST_NONE, 0, nullptr, 0, nullptr, nullptr))
		throw
			PathFileExists(m_infPath.c_str()) ?
			eHidInstINFDependencyMissing :
			eHidInstCopyOEMFail;
#endif

	// The SYSTEM device class is where the device will be installed
	SystemInfoClass hInfo;

	// See if we can find an already-extant hardware node.  If so, we'll just use that one.

	// We next create an empty devnode where the ocuhid legacy device may be attached.
	// This empty devnode will then be characterized with a PNPID (by us) and then we let PNP
	// find and load the driver from there.  This is basically what the add/remove hardware wizard
	// does when you add legacy hardware.
	NonPnpDevnode devInfo(hInfo);

  // First associate the driver with the new PNP devnode:
  devInfo.Associate();

  // Now we'll select the device:
  m_restartRequired = devInfo.InstallDriver();
}

eHidStatus CInstallerBase::ForEach(function<int (SP_DEVINFO_DATA&)> op)
{
	// Enumerate the root tree to find the one that must be updated
	hInfo = SetupDiGetClassDevsW(&GUID_DEVCLASS_SYSTEM, L"root\\system", nullptr, 0);

	// Set up the info structure that we will use to query for information
	SP_DEVINFO_DATA info;
	memset(&info, 0, sizeof(info));
	info.cbSize = sizeof(info);

	// Enumerate to the devnode matching our device ID:
	for(DWORD i = 0; SetupDiEnumDeviceInfo(hInfo, i, &info); i++)
	{
		wchar_t buf[MAX_PATH];
		DWORD dwType = REG_SZ;
		DWORD reqSize;

		// This is a routine that gets a requested device property.  The device
		// property we're interested in for this call is the hardware identifier,
		// because we'd like to match the hardware identifier to the hardware ID
		// that the Hid emulator uses
		if(!SetupDiGetDeviceRegistryPropertyW(
				hInfo,
				&info,
				SPDRP_HARDWAREID,
				&dwType,
				(LPBYTE)buf,
				sizeof(buf),
				&reqSize
			)
		)
			// Failed to get this HWID, try the next one.
			continue;

		if(!wcscmp(buf, sc_pnpID))
			return (eHidStatus)op(info);
	}

	return eHidInstNoDevsToUpdate;
}

void CInstallerBase::Update(void)
{
	// Update all detected devices.
	return ForEach(
		[this] (const SP_DEVINFO_DATA& data) -> int {
			BOOL bReboot;
			
			// This is an omnibus routine that will update a device if you provide its PNP ID
			// Though we don't strictly need to put this in a ForEach call, it is done anyway
			// to ensure that the routine isn't called when there isn't a need for it.
			if(!UpdateDriverForPlugAndPlayDevices(
					nullptr,
					sc_pnpID,
					m_infPath.c_str(),
					0,
					&bReboot
				)
			)
				return eHidInstUserCancel;
			return eHidInstSuccess;
		}
	);
}

void CInstallerBase::DeleteOcuHidService(const wchar_t* lpwcsName)
{
	eHidStatus rs = eHidInstSuccess;
	SC_HANDLE hSrv;

	// Open a handle to the HidEmulator service proper.
	hSrv = OpenServiceW(hMngr, L"HidEmulator", GENERIC_ALL);
	if(!hSrv)
		return eHidInstServiceOpenFailed;

	// Determine how many bytes will be needed to our service binaries.
	DWORD cbNeeded;
	QueryServiceConfigW(hSrv, NULL, 0, &cbNeeded);
	if(!cbNeeded)
		rs = eHidInstServiceConfQueryFail;

	// Path length acquired, query the service configuration, which will get us
	// the path to service binaries.
	vector<BYTE> configBuf(cbNeeded + 1);
	auto& config = (QUERY_SERVICE_CONFIGW&)configBuf[0];
	if(cbNeeded && !QueryServiceConfigW(hSrv, &config, cbNeeded, &cbNeeded))
		rs = eHidInstServiceConfQueryFail;

	// Attempt to delete the service:
	if(!DeleteService(hSrv))
		switch(GetLastError())
		{
		case ERROR_SERVICE_MARKED_FOR_DELETE:
			// Service already marked for deletion:
			break;
		default:
			rs = eHidInstServiceDeleteFailed;
		}

	// Done deleting the service, close the handle we don't need.
	CloseServiceHandle(hSrv);

	// Delete the service binaries from the system:
	{
		TCHAR wcSysRoot[MAX_PATH];
		TCHAR wcFullPath[MAX_PATH];

		// Recover system root
		GetWindowsDirectoryW(wcSysRoot, MAX_PATH);

		// Create the full path:
		swprintf_s(wcFullPath, ARRAYSIZE(wcFullPath), L"%s\\%s", wcSysRoot, config.lpBinaryPathName);

		// Verify that the full path exists before we try to delete it.
		if(PathFileExistsW(wcFullPath))
			// Delete the file now if we can
			if(!DeleteFileW(wcFullPath))
				// File in use.  Delay deletion until reboot.
				MoveFileEx(wcFullPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	}

	return rs;
}

void CInstallerBase::Uninstall(void)
{
	// Delete all OcuHid devnodes first
	eHidStatus rs = ForEach(
		[this] (SP_DEVINFO_DATA& data) -> int {
			SetupDiCallClassInstaller(DIF_REMOVE, hInfo, &data);
			return eHidInstSuccess;
		}
	);

	if(!SUCCEEDED(rs))
		// Devnode removal failed; do not attempt to remove the service.
		return rs;

	// Once the devnodes are gone, the OcuHid service must also be removed

	// Get a manager handle with all-access so deletion is possible
	hMngr = OpenSCManagerW(nullptr, SERVICES_ACTIVE_DATABASEW, SC_MANAGER_ALL_ACCESS);
	if(!hMngr)
		return eHidInstSCManOpenFailed;

	// Now the services proper may be deleted:
	DeleteOcuHidService(L"HidEmulator");
	DeleteOcuHidService(L"HidEmulatorKmdf");

	// Destructor will handle final cleanup
	return eHidInstSuccess;
}
