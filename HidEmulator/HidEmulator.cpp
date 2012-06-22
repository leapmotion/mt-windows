#include "stdafx.h"

// Forward declarations:
void Unload(PDRIVER_OBJECT DriverObject);
NTSTATUS AddDevice(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT PhysicalDeviceObject);
NTSTATUS PassThrough(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PowerPassThrough(PDEVICE_OBJECT DeviceObject, PIRP Irp);

// Driver entrypoint
extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	
	// Default almost everything:
	for each(auto*& p in DriverObject->MajorFunction)
		p = PassThrough;

	// Power IRP is special:
	DriverObject->MajorFunction[IRP_MJ_POWER] = PowerPassThrough;
	
	// AddDevice and Unload methods must be added, but here, do nothing.
	DriverObject->DriverExtension->AddDevice = AddDevice;
	DriverObject->DriverUnload = Unload;

	// Register with the HID minidriver.  No driver extension is required so its size is
	// set to zero.
	HID_MINIDRIVER_REGISTRATION reg;
	memset(&reg, 0, sizeof(reg));
	reg.Revision = HID_REVISION;
	reg.DriverObject = DriverObject;
	reg.RegistryPath = RegistryPath;
	reg.DeviceExtensionSize = 0;
	reg.DevicesArePolled = false;

	// Invoke the minidriver registration routine.  The reason we don't implement this
	// whole driver with the KMDF is because of this call:  The HID port driver wants
	// ownership of the device extension, and KMDF wants the same thing, so the two
	// contend.  Resolution of this contention is had by installing this driver as an
	// upper filter on the KMDF driver.
	status = HidRegisterMinidriver(&reg);
	if(!NT_SUCCESS(status))
	{
		OcsDebugPrint("Failed to register the mini driver (0x%08x)", status);
		return status;
	}

	OcsDebugPrint("OcuSpec WDF HID Emulator loaded");
	return status;
}

/// <summary>
/// Unload routine for the driver
/// </summary>
void Unload(PDRIVER_OBJECT DriverObject)
{
	OcsDebugPrint("OcuSpec HID Emulator unloaded");
}

NTSTATUS AddDevice(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT FunctionalDeviceObject)
{
	// Do nothing except return success
	OcsDebugPrint("WDF AddDevice invoked");
	return STATUS_SUCCESS;
}

NTSTATUS PassThrough(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	// The passthrough routine does just that:  It copies the current IRP stack location
	// to the next one, and allows the next driver in the chain to handle the request.
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(GET_NEXT_DEVICE_OBJECT(DeviceObject), Irp);
}

NTSTATUS PowerPassThrough(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	// The power pass-through operation is a little different.  We need to indicate to
	// the power manager that it can start the next power IRP (for this driver), and then
	// we copy the current stack location to next and handle things as expected.
	PoStartNextPowerIrp(Irp);
	IoCopyCurrentIrpStackLocationToNext(Irp);
	return PoCallDriver(GET_NEXT_DEVICE_OBJECT(DeviceObject), Irp);
}