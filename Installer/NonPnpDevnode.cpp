#include "stdafx.h"
#include "NonPnpDevnode.h"

NonPnpDevnode::NonPnpDevnode(HDEVINFO hInfo):
	m_hInfo(hInfo),
	released(false)
{
	memset((PSP_DEVINFO_DATA)this, 0, sizeof(SP_DEVINFO_DATA));
	cbSize = sizeof(SP_DEVINFO_DATA);

	if(!SetupDiCreateDeviceInfoW(hInfo, L"SYSTEM", &GUID_DEVCLASS_SYSTEM, nullptr, nullptr, DICD_GENERATE_ID, this))
		throw eHidInstDevCreateFail;
}

NonPnpDevnode::~NonPnpDevnode(void)
{
	if(!released)
		SetupDiCallClassInstaller(DIF_REMOVE, m_hInfo, this);
}

void NonPnpDevnode::Associate(void) {
	// Here's where the HWID is assigned.  This is how PNP knows what to attach to the newly created
	// devnode.
	if(!SetupDiSetDeviceRegistryPropertyW(m_hInfo, this, SPDRP_HARDWAREID, (LPCBYTE)sc_pnpID, sizeof(sc_pnpID)))
		throw eHidInstDevIDAssignFail;

	// Now, we need to let PNP know that this is a device, so that it will actually try to find drivers
	SetupDiCallClassInstaller(DIF_REGISTERDEVICE, hInfo, &devInfo);

	// Construct a set of supported drivers.  This list will include the driver that we installed earlier with
	// the earlier call to SetupCopyOEMInf.
	if(!SetupDiBuildDriverInfoList(hInfo, &devInfo, SPDIT_COMPATDRIVER))
		return eHidInstInfoListBuildFail;

	// Arbitrarily select the first driver.  There really should be only one driver anyway
	SP_DRVINFO_DATA driverInfo;
	memset(&driverInfo, 0, sizeof(driverInfo));
	driverInfo.cbSize = sizeof(driverInfo);
	if(!SetupDiEnumDriverInfo(hInfo, &devInfo, SPDIT_COMPATDRIVER, 0, &driverInfo))
		return eHidInstCompatDriverFindFail;

	// Assign the selected driver to this device.
	if(!SetupDiSetSelectedDriver(hInfo, &devInfo, &driverInfo))
		return eHidInstDriverSelectFail;

	DWORD bReboot = false;
#if OCU_USE_UPDATEDRIVER
	// Everything else is laid in.  Now, we just have to tell PNP to find the drivers for the device given
	// the INF that we supplied, and then let PNP do the rest.
	if(!UpdateDriverForPlugAndPlayDevices(nullptr, sc_pnpID, m_infPath.c_str(), INSTALLFLAG_FORCE, &bReboot))
		return eHidInstUserCancel;
#else

	// Now, even though we have been operating on devInfo for most of this function, we still need
	// to indicate to the SetupAPI that we want to call some class installers on this device by making
	// this call.  That's what this call does--it tells SetupAPI about our intent.
	SetupDiSetSelectedDevice(hInfo, &devInfo);

	// Install the driver we selected into the device we just selected.
	DWORD dwReboot = 0;
	if(!InstallSelectedDriver(nullptr, hInfo, nullptr, true, &dwReboot))
		return eHidInstInstallSelectionFailed;

	// Reboot necessity is based on the value of the reboot operation.
	bReboot = dwReboot == DI_NEEDREBOOT;
#endif

	return bReboot ? eHidInstRestartRequired : eHidInstSuccess;
}