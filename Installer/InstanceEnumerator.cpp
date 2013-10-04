#include "stdafx.h"
#include "InstanceEnumerator.h"

InstanceEnumerator::InstanceEnumerator(std::shared_ptr<SystemInfoClassBase> hInfo):
  m_hInfo(hInfo),
  m_i(0)
{
	// Set up the info structure that we will use to query for information
	memset(&m_info, 0, sizeof(m_info));
	m_info.cbSize = sizeof(m_info);
}

InstanceEnumerator::~InstanceEnumerator(void)
{
}

bool InstanceEnumerator::Next(void) {
	// Enumerate to the devnode matching our device ID:
	while(SetupDiEnumDeviceInfo(*m_hInfo, m_i++, &m_info))
	{
		wchar_t buf[MAX_PATH];
		DWORD dwType = REG_SZ;
		DWORD reqSize;

		// This is a routine that gets a requested device property.  The device
		// property we're interested in for this call is the hardware identifier,
		// because we'd like to match the hardware identifier to the hardware ID
		// that the Hid emulator uses
		if(!SetupDiGetDeviceRegistryPropertyW(
				*m_hInfo,
				&m_info,
				SPDRP_HARDWAREID,
				&dwType,
				(LPBYTE)buf,
				sizeof(buf),
				&reqSize
			)
		)
			// Failed to get this HWID, try the next one.
			continue;

		if(!wcscmp(buf, gc_pnpID))
			return true;
	}
  return false;
}

void InstanceEnumerator::DestroyCurrent(void) {
  SetupDiCallClassInstaller(DIF_REMOVE, *this, &Current());

  // Do we need to restart now?
  SP_DEVINSTALL_PARAMS params;
  params.cbSize = sizeof(params);
  SetupDiGetDeviceInstallParams(*this, &Current(), &params);

  if(params.Flags & (DI_NEEDREBOOT | DI_NEEDRESTART))
    RequireRestart();
}