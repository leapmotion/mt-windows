#include "stdafx.h"
#include "SystemInfoClass.h"

SystemInfoClassBase::SystemInfoClassBase(void):
	m_hInfo(INVALID_HANDLE_VALUE)
{
}

SystemInfoClassBase::~SystemInfoClassBase(void) {
	if(m_hInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(m_hInfo);
}