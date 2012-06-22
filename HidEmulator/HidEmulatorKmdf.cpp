#include "stdafx.h"
#include "Emulator.h"
#include "EmulatorContext.h"
#include "Usb.h"

// Fake IDs
#define VMULTI_POOL_TAG            (ULONG) 'escO'
#define VMULTI_HARDWARE_IDS        L"HID\\OcsEmulator\0\0"
#define VMULTI_HARDWARE_IDS_LENGTH sizeof(VMULTI_HARDWARE_IDS)

// Forward declarations:
extern "C" NTSTATUS __stdcall DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);

EVT_WDF_DRIVER_DEVICE_ADD EvtDeviceAdd;
EVT_WDF_DRIVER_UNLOAD EvtDriverUnload;
EVT_WDF_OBJECT_CONTEXT_CLEANUP EvtDriverContextCleanup;
NTSTATUS VMultiEvtWdmPreprocessMnQueryId(WDFDEVICE Device, PIRP Irp);
EVT_WDFDEVICE_WDM_IRP_PREPROCESS PreprocessMnQueryId;

// Allocation assurance:
#pragma alloc_text(INIT, DriverEntry)

// Driver entrypoint
NTSTATUS __stdcall DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	// Driver configuration only requires EvtDeviceAdd
	WDF_DRIVER_CONFIG config;
	WDF_DRIVER_CONFIG_INIT(&config, EvtDeviceAdd);
	config.EvtDriverUnload = EvtDriverUnload;

	// Set up the cleanup routine
	WDF_OBJECT_ATTRIBUTES  attributes;
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.EvtCleanupCallback = EvtDriverContextCleanup;

	// Initialize the WDF:
	status = WdfDriverCreate(DriverObject, RegistryPath, &attributes, &config, WDF_NO_HANDLE);
	if(!NT_SUCCESS(status))
	{
		OcsDebugPrint("WdfDriverCreate failed with 0x%08x", status);
		return status;
	}

	// Done!
	OcsDebugPrint("OcuSpec Kmdf HID lower filter loaded");
	return STATUS_SUCCESS;
}

/// <summary>
/// AddDevice routine
/// </summary>
NTSTATUS EvtDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit)
{
	NTSTATUS status;

	// We are a filter driver:
	WdfFdoInitSetFilter(DeviceInit);

	// Root-enumerated, so need to override some PNP handling.  This is needed because normally PNP
	// tries to determine the state of the hardware before spinning up the driver; unfortunately a
	// nondevice driver doesn't actually have any hardware, so we need to convince PNP that it needs
	// to load us in spite of this.
	UCHAR minorFunction = IRP_MN_QUERY_ID;
	status = WdfDeviceInitAssignWdmIrpPreprocessCallback(DeviceInit, PreprocessMnQueryId, IRP_MJ_PNP, &minorFunction, 1);
	if(!NT_SUCCESS(status))
		return status;

	// Create the device that we will use to handle all emulated inputs.  This device object will be
	// reachable through the HID class driver's device interface.  It won't be directly reachable, but
	// the HID class driver forwards the requests that we are interested in to us through our dispatch
	// routines.
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, CEmulator);
	attributes.EvtCleanupCallback = EvtEmulatorCleanup;
	WDFDEVICE hDevice;
	status = WdfDeviceCreate(&DeviceInit, &attributes, &hDevice);
	if(!NT_SUCCESS(status))
		return status;

	// Initialize the device context.  This initialization is performed using placement new, because
	// the memory required for the device context is already allocated.
	auto devContext = GetDeviceContext(hDevice);
	new (devContext) CEmulator(hDevice);

	// This is the externally-facing queue, it's used to handle requests that come down from the class
	// driver.  This queue is distinct from the internal queues used by CEmulator in that the CEmulator
	// internal queues are manually dispatched, whereas this one is dispatched automatically.
	WDFQUEUE queue;
	WDF_IO_QUEUE_CONFIG queueConfig;
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
	queueConfig.PowerManaged = WdfFalse;
	queueConfig.EvtIoInternalDeviceControl = EvtInternalDeviceControl;
	status = WdfIoQueueCreate(hDevice, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);

	return status;
}

NTSTATUS PreprocessMnQueryId(WDFDEVICE Device, PIRP Irp)
{
	NTSTATUS status;

    // Get a pointer to the current location in the Irp
    auto IrpStack = IoGetCurrentIrpStackLocation(Irp);

    // Get the device object
    auto DeviceObject = WdfDeviceWdmGetDeviceObject(Device); 

    // This check is required to filter out QUERY_IDs forwarded
    // by the HIDCLASS for the parent FDO. These IDs are sent
    // by PNP manager for the parent FDO if you root-enumerate this driver.
    auto previousSp = IrpStack + 1;

    if (previousSp->DeviceObject == DeviceObject) 
        // Filtering out this basically prevents the Found New Hardware
        // popup for the root-enumerated VMulti on reboot.
        status = Irp->IoStatus.Status;
    else switch(IrpStack->Parameters.QueryId.IdType)
	{
    case BusQueryDeviceID:
    case BusQueryHardwareIDs:
        // HIDClass is asking for child deviceid and hardwareids.
		{
			// Create arbitrary ID:
			auto buffer = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, VMULTI_HARDWARE_IDS_LENGTH, VMULTI_POOL_TAG);
			if(buffer) 
			{
				// Do the copy, store the buffer in the Irp
				RtlCopyMemory(buffer, VMULTI_HARDWARE_IDS, VMULTI_HARDWARE_IDS_LENGTH);
				Irp->IoStatus.Information = (ULONG_PTR)buffer;
				status = STATUS_SUCCESS;
			}
			else 
				//  No memory
				status = STATUS_INSUFFICIENT_RESOURCES;
			
			// We don't need to forward this to our bus. This query is for our
			// child so we should complete it right here.
			Irp->IoStatus.Status = status;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
		}
        break;

    default:
		// Default handling, just complete the request and return.
        status = Irp->IoStatus.Status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        break;
    }

    return status;
}

void EvtDriverContextCleanup(WDFOBJECT Driver)
{
	OcsDebugPrint("OcuSpec Kmdf HID cleanup initiated");
}

void EvtDriverUnload(WDFDRIVER Driver)
{
	OcsDebugPrint("OcuSpec Kmdf HID lower filter unloaded");
}
