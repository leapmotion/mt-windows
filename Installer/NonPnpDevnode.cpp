#include "stdafx.h"
#include "NonPnpDevnode.h"
#include "SystemInfoClass.h"
#include <iostream>
#include <vector>

using namespace std;

NonPnpDevnode::NonPnpDevnode(void):
	m_hInfo(nullptr),
	released(false)
{
	memset(&m_driverInfo, 0, sizeof(m_driverInfo));
	m_driverInfo.cbSize = sizeof(m_driverInfo);
}

NonPnpDevnode::NonPnpDevnode(std::shared_ptr<SystemInfoClass> hInfo):
	m_hInfo(hInfo),
	released(false)
{
	memset((PSP_DEVINFO_DATA)this, 0, sizeof(SP_DEVINFO_DATA));
	cbSize = sizeof(SP_DEVINFO_DATA);

  // Create an infoset to hold our single device:
	if(!SetupDiCreateDeviceInfoW(*hInfo, L"SYSTEM", &GUID_DEVCLASS_SYSTEM, nullptr, nullptr, DICD_GENERATE_ID, this))
		throw eHidInstDevCreateFail;

	// Here's where the HWID is assigned.  This is how PNP knows what to attach to the newly created devnode.
	if(!SetupDiSetDeviceRegistryPropertyW(*m_hInfo, this, SPDRP_HARDWAREID, (LPCBYTE)gc_pnpID, gc_pnpIDLen))
		throw eHidInstDevIDAssignFail;

	// Now, we need to let PNP know that this is a device, so that it will actually try to find drivers
	if(!SetupDiCallClassInstaller(DIF_REGISTERDEVICE, *m_hInfo, this))
    throw eHidInstDeviceRegistrationFailed;
}

NonPnpDevnode::NonPnpDevnode(std::shared_ptr<SystemInfoClass> hInfo, const SP_DEVINFO_DATA& data):
  SP_DEVINFO_DATA(data),
	m_hInfo(hInfo),
  released(false)
{
}

NonPnpDevnode::~NonPnpDevnode(void)
{
	if(!released)
		SetupDiCallClassInstaller(DIF_REMOVE, *m_hInfo, this);
}

void NonPnpDevnode::Associate(void) {
	// Construct a set of supported drivers.  This list will include the driver that we installed earlier with
	// the earlier call to SetupCopyOEMInf.
	if(!SetupDiBuildDriverInfoList(*m_hInfo, this, SPDIT_COMPATDRIVER))
		throw eHidInstInfoListBuildFail;

	// Select the best driver.  There really should be only one driver anyway
  DWORD i = 0;

  vector<char> buf;
  for(
    auto driverInfo = m_driverInfo;
    SetupDiEnumDriverInfo(*m_hInfo, this, SPDIT_COMPATDRIVER, i, &driverInfo);
    i++
  )
    if(
      !i ||

      m_driverInfo.DriverVersion < driverInfo.DriverVersion ||

      m_driverInfo.DriverVersion == driverInfo.DriverVersion &&
      (long long&)m_driverInfo.DriverDate < (long long&)m_driverInfo.DriverDate
    )
    {
      if(i) {
        // Obtain driver information detail:
        DWORD sz;
        SetupDiGetDriverInfoDetailW(*m_hInfo, this, &driverInfo, nullptr, 0, &sz);
        buf.resize(sz);
        
        // Obtain the full data:
        auto& data = (SP_DRVINFO_DETAIL_DATA_W&)buf[0];
        memset(&data, 0, sz);
        data.cbSize = sz;
        if(SetupDiGetDriverInfoDetailW(*m_hInfo, this, &driverInfo, &data, sz, &sz))
          // Uninstall the driver:
          SetupUninstallOEMInf(data.InfFileName, 0, nullptr);
        
        cout << "Ex2" << endl;
      }

      m_driverInfo = driverInfo;
    }

  if(!i)
    throw eHidInstCompatDriverFindFail;

  // Historical defect:
  //0x0001000000000000

	// Assign the selected driver to this device.
	if(!SetupDiSetSelectedDriver(*m_hInfo, this, &m_driverInfo))
		throw eHidInstDriverSelectFail;

	// Now, even though we have been operating on devInfo for most of this function, we still need
	// to indicate to the SetupAPI that we want to call some class installers on this device by making
	// this call.  That's what this call does--it tells SetupAPI about our intent.
	if(!SetupDiSetSelectedDevice(*m_hInfo, this))
    throw eHidInstFailedToSelectDevice;
}


bool NonPnpDevnode::InstallDriver(void) {
	// Install the driver we selected into the device we just selected.
	DWORD dwReboot = 0;
	if(!InstallSelectedDriver(nullptr, *m_hInfo, nullptr, true, &dwReboot))
		throw eHidInstInstallSelectionFailed;

	// Reboot necessity is based on the value of the reboot operation.
  released = true;
	return dwReboot == DI_NEEDREBOOT;
}