#include "stdafx.h"
#include "SystemInfoClass.h"

SystemInfoClass::SystemInfoClass(void):
	m_hInfo(INVALID_HANDLE_VALUE)
{
	// The SYSTEM device class is where the device will be installed
	m_hInfo = SetupDiCreateDeviceInfoListExW(&GUID_DEVCLASS_SYSTEM, nullptr, nullptr, nullptr);
	if(m_hInfo == INVALID_HANDLE_VALUE)
		throw eHidInstSysClassNotFound;
}

SystemInfoClass::~SystemInfoClass(void)
{
	if(m_hInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(m_hInfo);
}
