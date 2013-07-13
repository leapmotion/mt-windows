#pragma once

#define KMDF_VERSION_MAJOR 1

#include <initguid.h>
#include <ntifs.h>
#include <windef.h>

// WDF stuff:
#include <wdf.h>
#include "usbdi.h"
#include "usbdlib.h"
#include <wdfusb.h>

extern "C" {
	#include <hidport.h>
}

#if DBG
	#define OcsDebugPrint(msg, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, msg "\n", __VA_ARGS__)
#else
	#define OcsDebugPrint(msg, ...)
#endif

// Marker definitions, defined to nothing here to prevent compilation malfunctions
#define __INCLUDE_FROM_USB_DRIVER
#define __INCLUDE_FROM_HID_DRIVER
#define USB_CAN_BE_DEVICE

#define ATTR_PACKED
#define ATTR_CONST
#define ATTR_NON_NULL_PTR_ARG(x)

#define GET_NEXT_DEVICE_OBJECT(DO) \
    (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->NextDeviceObject)

// Vendor identifier (must be requested)
#define OEM_VENDOR_ID	'SUCO'

// Some generic typdefs, for use by the HID headers
typedef char int8_t;
typedef BYTE uint8_t;
typedef WORD uint16_t;
typedef DWORD uint32_t;

// Forward declaration of placement new
inline void* operator new(size_t, void* p) {return p;}

// Internal headers:
#include "../Globals/Interface.h"
