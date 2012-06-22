#include "StdAfx.h"
#include "OcuInterface.h"
#include "OcuHidInstance.h"
#include <iostream>

using namespace std;

// These are preprocessor definitions copied from Windows kernel-mode header files.  They're pasted here in
// order to make it 
#define FILE_DEVICE_KEYBOARD             0x0000000b
#define METHOD_NEITHER                   3
#define FILE_ANY_ACCESS                  0

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define HID_CTL_CODE(id)				 CTL_CODE(FILE_DEVICE_KEYBOARD, (id), METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_HID_GET_DEVICE_DESCRIPTOR  HID_CTL_CODE(0)

COcuInterface::COcuInterface(bool bAsynchronous):
	m_lastErr(0),
	m_bAsynchronous(bAsynchronous)
{
	// Recover the GUID of the HID device class.  This GUID identifies all HID-compliant devices on
	// the system, and OcuSpec is among them.
	GUID hidGuid;
	HidD_GetHidGuid(&hidGuid);

	// Open handle to root PNP node.  The root PNP node is the parent of all PNP devices, and
	// it's called /root.  The OcuSpec emulator device isn't a true device, so its parent will
	// be the root node.
	auto root = SetupDiGetClassDevs(&hidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if(root == INVALID_HANDLE_VALUE)
	{
		m_lastErr = GetLastError();
		return;
	}

	// Set up the structure to get device interface data.  The device interface provides a standardized
	// way of naming a device driver for access by a usermode client.  HID defines its own device interface
	// and that's the one we will be using.
	SP_DEVICE_INTERFACE_DATA deviceInfoData;
	deviceInfoData.cbSize = sizeof(deviceInfoData);

	// Begin enumerating all known devices on the system.
	vector<BYTE> buf;
	for(
		DWORD i = 0;
		SetupDiEnumDeviceInterfaces(root, nullptr, &hidGuid, i, &deviceInfoData);
		i++
	)
	{
		// First, we need to get the length of the detail information that will be required.
		DWORD reqLen = 0;
		SetupDiGetDeviceInterfaceDetail(root, &deviceInfoData, nullptr, 0, &reqLen, nullptr);

		// Now that we know the required length, we allocate a buffer and read the details.
		buf.resize(reqLen);
		SP_DEVICE_INTERFACE_DETAIL_DATA& detail = (SP_DEVICE_INTERFACE_DETAIL_DATA&)buf[0];
		detail.cbSize = sizeof(detail);
		if(!SetupDiGetDeviceInterfaceDetail(root, &deviceInfoData, &detail, reqLen, &reqLen, nullptr))
			continue;

		// Now that we have the path to device, we can try to open it for writing.  The only method
		// we use on the returned handle is DeviceIoControl, so the GENERIC_WRITE is the only permission
		// that we need.
		HANDLE hDev = CreateFile(
			detail.DevicePath,
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			m_bAsynchronous ? FILE_FLAG_OVERLAPPED : 0,
			nullptr
		);
		if(hDev == INVALID_HANDLE_VALUE)
			continue;

		// We were able to open this device.  At this point, it may or may not be a HID device--we
		// won't know until later--but we need to record this handle for now.
		push_back(new COcuHidInstance(hDev, m_bAsynchronous));
	}
}

COcuInterface::~COcuInterface(void)
{
	// Destructor call.  Any unclaimed interface must be destroyed
	// to prevent resource leaks.
	for each(auto q in *this)
		delete q;
}

COcuHidInstance* COcuInterface::GetFirstCompliantInterface(void)
{
	eHidStatus rs;

	// All we do here is to enumerate our own local set of devices
	// and then return the first device that appears to initialize
	// correctly.  This routine is offered as a convenience on those
	// systems were it is well known that only one OcuEmulator device
	// will be present.
	for(auto q = begin(); q != end(); q++)
	{
		auto cur = *q;

		// If we succeed in initializing a particular instance, we remove
		// it from our collection and reeturn it.
		if(SUCCEEDED(rs = cur->Initialize()))
			return erase(q), cur;
	}
	
	// Nothing found, return here.
	return nullptr;
}