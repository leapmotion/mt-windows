#pragma once

extern "C" {

BOOL WINAPI InstallSelectedDriver(
		HWND hwndParent,
		HDEVINFO DeviceInfoSet,
		LPCTSTR Reserved,
		BOOL Backup,
		PDWORD bReboot
	);

}