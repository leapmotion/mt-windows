#include "stdafx.h"
#include "NonPnpDevnode.h"
#include "SystemInfoClass.h"

NonPnpDevnode::NonPnpDevnode(void):
	m_hInfo(nullptr),
	released(false)
{
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

	// Arbitrarily select the first driver.  There really should be only one driver anyway
	SP_DRVINFO_DATA driverInfo;
	memset(&driverInfo, 0, sizeof(driverInfo));
	driverInfo.cbSize = sizeof(driverInfo);
	if(!SetupDiEnumDriverInfo(*m_hInfo, this, SPDIT_COMPATDRIVER, 0, &driverInfo))
		throw eHidInstCompatDriverFindFail;

	// Assign the selected driver to this device.
	if(!SetupDiSetSelectedDriver(*m_hInfo, this, &driverInfo))
		throw eHidInstDriverSelectFail;

	DWORD bReboot = false;

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