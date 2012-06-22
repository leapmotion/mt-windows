#include "StdAfx.h"
#include "Emulator.h"
#include "EmulatorContext.h"
#include "WdfObject.h"
#include "WdfObjectDeleter.h"
#include "WdfObjectLock.h"
#include "WdfRequestRequeuer.h"
#include "WdfRequestCompleter.h"

NTSTATUS SendSyncIrp(PDEVICE_OBJECT DevObj, PIRP Irp, BOOLEAN fCopyToNext);

CEmulator::CEmulator(WDFDEVICE hDevice):
	m_hDevice(hDevice),
	IoTarget(WdfDeviceGetIoTarget(hDevice)),
	InputMode(MODE_MULTI_TOUCH)
{
	WDF_IO_QUEUE_CONFIG config;
	WDF_IO_QUEUE_CONFIG_INIT(&config, WdfIoQueueDispatchManual);

	// The synthesis queue is used when a synthesis request has been made, but the
	// read queue is currently empty.  If the read queue is not currently empty,
	// the synthesis request directly completes one of the elements in the read queue.
	WdfIoQueueCreate(hDevice, &config, WDF_NO_OBJECT_ATTRIBUTES, &SynthQueue);

	// The read queue is what holds requests that come from the class driver.  If
	// there is anything in the synthesis queue when a read request is made, then
	// the read request is completed immediately, but typically the read request
	// will wind up in this queue and wait there for awhile.
	WdfIoQueueCreate(hDevice, &config, WDF_NO_OBJECT_ATTRIBUTES, &ReadQueue);
}

CEmulator::~CEmulator(void)
{
	// No cleanup needed.  WDF will tear down anything we created because the things we created
	// are bound to the device object.
}

void EvtEmulatorCleanup(WDFOBJECT Device)
{
	// Acquire the device context
	auto dev = GetDeviceContext(Device);

	// Invoke the destructor directly, rather than with delete, because we cannot deallocate
	// memory that is in a device context block.
	dev->~CEmulator();
}

void CEmulator::Satisfy(WDFREQUEST ReadRequest, WDFREQUEST SynthRequest, bool bSynthesisInitiator)
{
	WDFMEMORY memory;
	CWdfRequestRequeuer rq(ReadRequest, bSynthesisInitiator ? nullptr : ReadQueue);
	CWdfRequestCompleter term(SynthRequest);
	NTSTATUS& status = rq.Status;
	
	// Get the destination buffer
	status = WdfRequestRetrieveOutputMemory(ReadRequest, &memory);
	if(!NT_SUCCESS(status))
		return term.SetStatus(status);

	// Extract the synthesis request and buffer
	auto packet = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(SynthRequest)->UserBuffer;
	auto& data = (HID_SYNTEHSIZE_REPORT&)*packet->reportBuffer;
	if(packet->reportBufferLen < sizeof(data))
		return term.SetStatus(STATUS_BUFFER_TOO_SMALL);

	// Decide what to copy, and how much to be copied.
	switch(data.SynthesizedReportID)
	{
	case REPORTID_FINGERS:
		{
			// Obtain reference to the report:
			auto& report = data.GestureReport;
			WdfRequestSetInformation(ReadRequest, sizeof(report));
			status = WdfMemoryCopyFromBuffer(memory, 0, &report, sizeof(report));

			// Update touch state:
			ULONG ct = min(2, report.ActualInputs);
			for(size_t i = 0; i < ct; i++)
				if(report.contacts[i].bContactId < 16)
					m_state.touchState[report.contacts[i].bContactId] = report.contacts[i];
		}
		break;
	case REPORTID_MOUSE:
		m_state.mouseState = data.MouseReport;
		WdfRequestSetInformation(ReadRequest, sizeof(m_state.mouseState));
		status = WdfMemoryCopyFromBuffer(memory, 0, &m_state.mouseState, sizeof(m_state.mouseState));
		break;
	case REPORTID_NULL:
		// We don't do anything with this, this report type is just to validate connectivity.
		OcsDebugPrint("Null report request received.");
		break;
	}
}

template<bool bIsSynthesis>
void CEmulator::SatisfyOrEnqueue(WDFREQUEST Request)
{
	if(bIsSynthesis)
		OcsDebugPrint("Output report request received");
	else
		OcsDebugPrint("Report read request received");

	WDFREQUEST SecondaryRequest;
	const WDFQUEUE& Queue = bIsSynthesis ? ReadQueue : SynthQueue;
	{
		// Lock the read queue:
		CWdfObjectLock lk(ReadQueue);

		// Determine if there is anything waiting to be synthesized.
		NTSTATUS status = WdfIoQueueRetrieveNextRequest(Queue, &SecondaryRequest);
		if(!NT_SUCCESS(status))
			// Nothing waiting, we have to queue and return.
		{
			status = WdfRequestForwardToIoQueue(Request, ReadQueue);
			if(!NT_SUCCESS(status))
				// Something went wrong.  Complete the request here.
				WdfRequestComplete(Request, status);
			return;
		}
	}

	// We have a synthesis request.  Satisfy the read request.
	if(bIsSynthesis)
		Satisfy(SecondaryRequest, Request, true);
	else
		Satisfy(Request, SecondaryRequest, false);
}

void CEmulator::GetDeviceDescriptor(WDFREQUEST Request)
{
	CWdfRequestCompleter rq(Request);
	NTSTATUS& status = rq.Status;

	// Retrieves the device's HID descriptor.
	OcsDebugPrint("Device descriptor requested");
	
	// Get the memory object:
	WDFMEMORY memory;
	status = WdfRequestRetrieveOutputMemory(Request, &memory);
	if(!NT_SUCCESS(status))
		return;

	// Copy in the descriptor:
	status = WdfMemoryCopyFromBuffer(memory, 0, &g_hidDesc, sizeof(g_hidDesc));
	if(!NT_SUCCESS(status))
		return;

	// Done, report size and complete
	status = STATUS_SUCCESS;
	WdfRequestSetInformation(Request, sizeof(g_hidDesc));
}

void CEmulator::GetDeviceAttributes(WDFREQUEST Request)
{
	CWdfRequestCompleter rq(Request);
	NTSTATUS& status = rq.Status;

	OcsDebugPrint("Device attributes requested");

	WDFMEMORY memory;
	status = WdfRequestRetrieveOutputMemory(Request, &memory);
	if(!NT_SUCCESS(status))
		return;

	// Copy over attributes:
	HID_DEVICE_ATTRIBUTES attribs;
	attribs.Size = sizeof(attribs);
	attribs.VendorID = OCUHID_VENDOR_ID;
	attribs.ProductID = OCUHID_EMULATED_DEVICE;
	attribs.VersionNumber = OCUHID_INTERFACE_VERSION;
	status = WdfMemoryCopyFromBuffer(memory, 0, &attribs, sizeof(attribs));
	if(!NT_SUCCESS(status))
		return;

	WdfRequestSetInformation(Request, sizeof(attribs));
}

void CEmulator::GetReportDescriptor(WDFREQUEST Request)
{
	CWdfRequestCompleter rq(Request);
	NTSTATUS& status = rq.Status;

	OcsDebugPrint("Report descriptor requested");

	// Get the information:
	WDFMEMORY memory;
	status = WdfRequestRetrieveOutputMemory(Request, &memory);
	if(!NT_SUCCESS(status))
	{
		OcsDebugPrint("Failed to get the output memory for the report descriptor");
		return;
	}

	status = WdfMemoryCopyFromBuffer(memory, 0, g_reportDesc, sizeof(g_reportDesc));
	if(!NT_SUCCESS(status))
	{
		OcsDebugPrint("Insufficient buffer to transfer the report descriptor");
		return;
	}

	status = STATUS_SUCCESS;
	WdfRequestSetInformation(Request, sizeof(g_reportDesc));
}

void CEmulator::GetString(WDFREQUEST Request)
{
	// Report received
	auto loc = *IoGetCurrentIrpStackLocation(WdfRequestWdmGetIrp(Request));
			
	UNICODE_STRING dest =
	{
		0,
		(USHORT)loc.Parameters.DeviceIoControl.OutputBufferLength / 2,
		(wchar_t*)WdfRequestWdmGetIrp(Request)->UserBuffer
	};

	// Ignore the language, switch on the least-two bytes:
	UNICODE_STRING src;
	switch((int)(long long)loc.Parameters.DeviceIoControl.Type3InputBuffer & 0xFFFF)
	{
	case HID_STRING_ID_IMANUFACTURER:
		RtlInitUnicodeString(&src, L"OcuSpec Manuf");
		break;
	case HID_STRING_ID_IPRODUCT:
		RtlInitUnicodeString(&src, L"OcuSpec Product");
		break;
	case HID_STRING_ID_ISERIALNUMBER:
		RtlInitUnicodeString(&src, L"OcuSpec Serial");
		break;
	default:
		OcsDebugPrint("Unrecognized string requested");
		break;
	}
	RtlCopyUnicodeString(&dest, &src);
	WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, dest.Length * 2 + 2);
}

void CEmulator::GetFeature(WDFREQUEST Request)
{
	CWdfRequestCompleter rq(Request);
	WDF_REQUEST_PARAMETERS params;
	WDF_REQUEST_PARAMETERS_INIT(&params);
	WdfRequestGetParameters(Request, &params);
	auto FeaturePacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;

	if(!FeaturePacket)
		return rq.SetStatus(STATUS_INVALID_DEVICE_REQUEST);
	if(params.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET))
		return rq.SetStatus(STATUS_BUFFER_TOO_SMALL);

	auto& InputReport = (HID_INPUT_REPORT&)*FeaturePacket->reportBuffer;
	switch(InputReport.ReportID)
	{
	case REPORTID_MAXIMUMCOUNT:
		if(FeaturePacket->reportBufferLen < sizeof(HID_MAXIMUM_COUNT))
			return rq.SetStatus(STATUS_INVALID_DEVICE_REQUEST);

		{
			auto& report = (HID_MAXIMUM_COUNT&)*FeaturePacket->reportBuffer;
			report.MaxCount = 16;
			report.DeviceIndex = 0;
			WdfRequestSetInformation(Request, sizeof(report));
		}
		OcsDebugPrint("MaximumCount requested");
		break;
	case REPORTID_FEATURE:
		if(FeaturePacket->reportBufferLen < sizeof(HID_INPUT_MODE))
			return rq.SetStatus(STATUS_INVALID_DEVICE_REQUEST);

		{
			auto& report = (HID_INPUT_MODE&)*FeaturePacket->reportBuffer;
			report.InputMode = InputMode;
			WdfRequestSetInformation(Request, sizeof(report));
		}
		OcsDebugPrint("Current input mode requested, currently %i", InputMode);
		break;
	default:
		OcsDebugPrint("Unrecognized feature report requested");
		rq.SetStatus(STATUS_REQUEST_NOT_ACCEPTED);
		break;
	}
}

void CEmulator::SetFeature(WDFREQUEST Request)
{
	CWdfRequestCompleter rq(Request);
	WDF_REQUEST_PARAMETERS params;
	WDF_REQUEST_PARAMETERS_INIT(&params);
	WdfRequestGetParameters(Request, &params);
	auto FeaturePacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;

	if(!FeaturePacket)
		return rq.SetStatus(STATUS_INVALID_DEVICE_REQUEST);
	if(params.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET))
		return rq.SetStatus(STATUS_BUFFER_TOO_SMALL);

	auto& InputReport = (HID_INPUT_REPORT&)*FeaturePacket->reportBuffer;
	switch(InputReport.ReportID)
	{
	case REPORTID_MAXIMUMCOUNT:
		rq.SetStatus(STATUS_INVALID_PARAMETER);
		OcsDebugPrint("Request made to set the maximum touch count; this is invalid");
		break;
	case REPORTID_FEATURE:
		if(sizeof(HID_INPUT_MODE) < FeaturePacket->reportBufferLen)
			return rq.SetStatus(STATUS_INVALID_DEVICE_REQUEST);

		WdfRequestSetInformation(Request, 0);
		{
			auto& report = (HID_INPUT_MODE&)*FeaturePacket->reportBuffer;
			DevInMode mode = (DevInMode)report.InputMode;
			switch(mode)
			{
			case MODE_MOUSE:
				OcsDebugPrint("Device %i input mode set to mouse", report.DeviceIndex);
				break;
			case MODE_SINGLE_TOUCH:
				OcsDebugPrint("Device %i input mode set to single touch", report.DeviceIndex);
				break;
			case MODE_MULTI_TOUCH:
				OcsDebugPrint("Device %i input mode set to multitouch", report.DeviceIndex);
				break;
			default:
				OcsDebugPrint("Input mode %i not recognized", InputMode);
				return rq.SetStatus(STATUS_INVALID_PARAMETER);
			}
			InputMode = mode;
		}
		break;
	default:
		OcsDebugPrint("Unrecognized report ID 0x%08x", InputReport.ReportID);
		rq.SetStatus(STATUS_INVALID_PARAMETER);
		break;
	}
}

void RequestDeleter(WDFREQUEST Request, WDFIOTARGET Target, PWDF_REQUEST_COMPLETION_PARAMS Params, WDFCONTEXT Context)
{
	WdfObjectDelete(Request);
}

NTSTATUS CEmulator::QueueSynthesisToSelf(const HID_SYNTEHSIZE_REPORT& report)
{
	NTSTATUS status;
	WDFREQUEST Request;

	{
		// Attributes set the target queue as the parent of the request about to be created
		WDF_OBJECT_ATTRIBUTES attribs;
		WDF_OBJECT_ATTRIBUTES_INIT(&attribs);
		attribs.ParentObject = IoTarget;

		// Create:
		status = WdfRequestCreate(&attribs, IoTarget, &Request);
		if(!NT_SUCCESS(status))
			return status;
	}

	// Set the completion routine, so that the request can be cleaned up:
	WdfRequestSetCompletionRoutine(Request, RequestDeleter, this);

	// Delete if something goes wrong before we're finished:
	CWdfObjectDeleter rd(Request);

	// Construct the memory to serve as the input.  The memory is based on the
	// request, so we set the request as the parent.  It will be cleaned up
	// automatically whenever the request eventually is deleted
	WDFMEMORY InputBuffer;
	{
		WDF_OBJECT_ATTRIBUTES attribs;
		WDF_OBJECT_ATTRIBUTES_INIT(&attribs);
		attribs.ParentObject = Request;

		status = WdfMemoryCreate(&attribs, NonPagedPool, 0, sizeof(report), &InputBuffer, nullptr);
		if(!NT_SUCCESS(status))
			return status;
	}
	
	// Fill the input buffer:
	status = WdfMemoryCopyFromBuffer(InputBuffer, 0, (void*)&report, sizeof(report));
	if(!NT_SUCCESS(status))
		return status;

	// Request is a write-only requets
	WDFMEMORY_OFFSET InputBufferOffset = {0, sizeof(report)};
	status = WdfIoTargetFormatRequestForIoctl(
		IoTarget,
		Request,
		IOCTL_HID_SET_OUTPUT_REPORT,
		InputBuffer,
		&InputBufferOffset,
		nullptr,
		nullptr
	);
	if(!NT_SUCCESS(status))
		return status;

	// Send the request off to the driver for processing:
	if(!WdfRequestSend(Request, IoTarget, WDF_NO_SEND_OPTIONS))
		return WdfRequestGetStatus(Request);

	// Succeeded.
	rd.Steal();
	return STATUS_SUCCESS;
}

void CEmulator::EvtInternalDeviceControl(WDFQUEUE Queue, WDFREQUEST Request, size_t OutputBufferLength, size_t InputBufferLength, ULONG IoControlCode)
{
	NTSTATUS status = STATUS_NOT_SUPPORTED;

	switch(IoControlCode)
	{
	case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
		return GetDeviceDescriptor(Request);
		
	case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
		return GetDeviceAttributes(Request);
		
	case IOCTL_HID_GET_REPORT_DESCRIPTOR:
		return GetReportDescriptor(Request);

	case IOCTL_HID_GET_STRING:
		return GetString(Request);
		
	case IOCTL_HID_SET_FEATURE:
		return SetFeature(Request);
		
	case IOCTL_HID_GET_FEATURE:
		return GetFeature(Request);

	case IOCTL_HID_READ_REPORT:
		return SatisfyOrEnqueue<false>(Request);

	case IOCTL_HID_SET_OUTPUT_REPORT:
		return SatisfyOrEnqueue<true>(Request);
	}

	// No idea what to do here.
	WdfRequestComplete(Request, status);
}

void EvtInternalDeviceControl(WDFQUEUE Queue, WDFREQUEST Request, size_t OutputBufferLength, size_t InputBufferLength, ULONG IoControlCode)
{
	auto Device = WdfIoQueueGetDevice(Queue);
	auto devContext = GetDeviceContext(Device);
	return devContext->EvtInternalDeviceControl(Queue, Request, OutputBufferLength, InputBufferLength, IoControlCode);
}
