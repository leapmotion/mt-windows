#pragma once
#pragma pack(push, 1)

#define __INCLUDE_FROM_USB_DRIVER
#define __INCLUDE_FROM_HID_DRIVER
#define USB_CAN_BE_DEVICE

#include "HID.h"
#include "HIDReportData.h"
#include "HidReportDescriptor.h"

/// <summary>
/// This is a static global definition of the HID descriptor.
/// </summary>
/// <remarks>
/// The HID descriptor is a top-level descriptor that provides one piece of information:
/// A description of the report descriptor.  The report descriptor has the rest of the
/// information about the device.
/// </remarks>
static USB_HID_StdDescriptor_HID_t g_hidDesc =
{
	0x09,					// length of HID descriptor
	0x21,					// descriptor type == HID  0x21
	0x0100,					// hid spec release
	0x00,					// country code == Not Specified
	0x01,					// number of HID class descriptors

	HID_DTYPE_Report,		// descriptor type 
	sizeof(g_reportDesc)	// total length of report descriptor
};

// The length of the report descriptor is pretty important as it is a spec-defined value.
// This assertion ensures that the length constraint is met.
static_assert(sizeof(g_hidDesc) == 9, "HID descriptor size not precisely 9");

#pragma pack(pop)
