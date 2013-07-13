#include "stdafx.h"
#include "ServiceControlManager.h"
#include "ServiceHandle.h"
#include <vector>

using namespace std;

ServiceControlManager::ServiceControlManager(void):
  m_hMngr(OpenSCManagerW(nullptr, SERVICES_ACTIVE_DATABASEW, SC_MANAGER_ALL_ACCESS)),
  m_restartRequired(false)
{
  if(!m_hMngr)
    throw eHidInstSCManOpenFailed;
}

ServiceControlManager::~ServiceControlManager(void)
{
  if(m_hMngr)
    CloseHandle(m_hMngr);
}

void ServiceControlManager::DeleteOcuHidService(const wchar_t* lpwcsName)
{
  QUERY_SERVICE_CONFIGW config;
  {
	  // Open a handle to the HidEmulator service proper.
    ServiceHandle hSrv(*this, L"HidEmulator");

	  // Determine how many bytes will be needed to our service binaries.
	  DWORD cbNeeded;
	  QueryServiceConfigW(hSrv, NULL, 0, &cbNeeded);
	  if(!cbNeeded)
		  throw eHidInstServiceConfQueryFail;

	  // Path length acquired, query the service configuration, which will get us
	  // the path to service binaries.
	  vector<BYTE> configBuf(cbNeeded + 1);
	  config = (QUERY_SERVICE_CONFIGW&)configBuf[0];
	  if(cbNeeded && !QueryServiceConfigW(hSrv, &config, cbNeeded, &cbNeeded))
		  throw eHidInstServiceConfQueryFail;

	  // Attempt to delete the service:
	  if(!DeleteService(hSrv))
		  switch(GetLastError())
		  {
		  case ERROR_SERVICE_MARKED_FOR_DELETE:
			  // Service already marked for deletion, nothing further required at this point.
			  break;
		  default:
			  throw eHidInstServiceDeleteFailed;
		  }
  }

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
}