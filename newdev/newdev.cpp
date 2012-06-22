#include "stdafx.h"
#include "newdev.h"

BOOL WINAPI InstallSelectedDriver(
		HWND hwndParent,
		HDEVINFO DeviceInfoSet,
		LPCTSTR Reserved,
		BOOL Backup,
		PDWORD bReboot
	)
{
	// This return will never be hit.  The only purpose of this project is to enable dependencies
	// to dynamically link (instead of delay-load) with newdev.dll
	return false;
}
