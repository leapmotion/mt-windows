#include "StdAfx.h"
#include "OcuHidInstance.h"
#include "AutoLocker.h"
#include "OcuIcon.h"
#include "OcuImage.h"
#include <iostream>

using namespace std;

COcuHidInstance::COcuHidInstance(HANDLE hDevice, bool bAsynchronous):
	m_hDevice(hDevice),
	m_bIsOcuHid(false),
	m_bAsynchronous(bAsynchronous),
	m_hCompletionPort(nullptr),
	m_hCompletionThread(nullptr),
	m_dwCompletionTid(0),
	m_hLock(CreateMutex(nullptr, false, nullptr))
{
}

COcuHidInstance::~COcuHidInstance(void)
{
	if(m_hCompletionPort)
	{
		// Completion port is created, we need to close it
		if(m_hCompletionThread)
		{
			// Compltion thread exists.  We need to signal it to exit, and then close the handle.
			PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, nullptr);

			// Wait for the completion thread to cycle through and exit
			WaitForSingleObject(m_hCompletionThread, INFINITE);

			// Close the completion port handle
			CloseHandle(m_hCompletionThread);
		}

		// Close the actual completion port
		CloseHandle(m_hCompletionPort);
	}

	// Close the bound device
	CloseHandle(m_hDevice);
}

DWORD COcuHidInstance::GetVersion(void) const {
	return 0;
}

eHidStatus COcuHidInstance::Initialize(void)
{
	if(m_bAsynchronous)
	{
		// Asynchronous operation.  A completion port is necessary, and a thread to dispatch the
		// completion port must be created.

		// Create the completion port for the device:
		m_hCompletionPort = CreateIoCompletionPort(m_hDevice, nullptr, (ULONG_PTR)this, 0);
		if(!m_hCompletionPort)
			return eHidIntrCompletionPortCreateFail;
		
		// Create the thread to wait on the completion port:
		m_hCompletionThread = CreateThread(nullptr, 0, &CompletionThread, this, 0, &m_dwCompletionTid);
		if(!m_hCompletionThread)
			return eHidIntrCompletionThreadNotCreated;
	}
	
	// Get information about this device.  Note that GetAttributes generates an IOCTL
	// that never pends, making it safe to call without an I/O status block.
	HIDD_ATTRIBUTES Attributes;
	if(!HidD_GetAttributes(m_hDevice, &Attributes))
		// Failed to get HID attributes
		return eHidIntrHidAttributeQueryFail;

	// Verify that the vendor attributes match up with our vendor ID, product, and version number:
	m_bIsOcuHid =
		Attributes.VendorID == OCUHID_VENDOR_ID &&
		Attributes.ProductID == OCUHID_EMULATED_DEVICE &&
		Attributes.VersionNumber == OCUHID_INTERFACE_VERSION;
	if(!m_bIsOcuHid)
		return eHidIntrNotOcuHid;

	// Send a NULL report.  This is a report recognized specifically by our driver type.
	// If the driver responds, it is properly configured and it is a compliant HidEmulator device.
	HID_SYNTEHSIZE_REPORT buf;
	memset(&buf, 0, sizeof(buf));
	buf.ReportID = REPORTID_SYNTHESIZE;
	buf.SynthesizedReportID = REPORTID_NULL;
	m_bIsOcuHid = SendOutputReport(buf, true);

	if(!m_bIsOcuHid)
		return eHidIntrNotRespondingProperly;
	return eHidIntrSuccess;
}

void COcuHidInstance::LiftAll(UCHAR nPoints) const
{
	// Initialize the synthesis structure
	HID_SYNTEHSIZE_REPORT report;
	memset(&report, 0, sizeof(report));
	report.GestureReport.ActualInputs = 1;
	report.GestureReport.contacts[0].InRange = true;

	// Synthesize a touch-up event for each contact point:
	while(nPoints--)
	{
		// Send the report on the requested contact identifier:
		report.GestureReport.contacts[0].bContactId = nPoints;
		SendOutputReport(report);
	}
}

DWORD COcuHidInstance::DoCompletionThread(void)
{
	DWORD numberOfBytes;
	ULONG_PTR key;
	COverlapped* pOverlapped;

	// Return any completed packets back to the overlap pool:
	while(
		GetQueuedCompletionStatus(
			m_hCompletionPort,
			&numberOfBytes,
			&key,
			(LPOVERLAPPED*)&pOverlapped,
			INFINITE
		)
	)
		// If the overlapped structure is set to a non-null value, then we need to wait on
		// the overlapped structure to finish.  Otherwise, we are being signalled to terminate.
		if(pOverlapped)
			CAutoLocker(m_hLock), m_overlapPool.Free(pOverlapped);
		else
			// Special signal from the destructor that we are shutting down.
			// Break out of the loop.
			break;

	// Done.
	return 0;
}

bool COcuHidInstance::SendOutputReport(const HID_SYNTEHSIZE_REPORT& report, bool block) const
{
	COverlapped* pOverlapped = nullptr;
	if(m_bAsynchronous)
		// Get a new overlapped structure to be used for this asynchronous operation
		pOverlapped = (CAutoLocker(m_hLock), m_overlapPool.Create());

	// Send the IOCTL:
	DWORD dwReturned;
	BOOL rs = DeviceIoControl(
		m_hDevice,						// The target device to get the IOCTL
		IOCTL_HID_SET_OUTPUT_REPORT,	// Output report is the only way we really have to communicate
		(void*)&report,					// Sending the report itself
		sizeof(report),					// Size of the report
		nullptr,						// No response necessary
		0,								// Zero response buffer size
		&dwReturned,					// Get the return buffer
		pOverlapped						// Overlapped structure, as needed
	);

	return
		// Was the IOCTL successful?
		rs ?
		(
			// Have we been asked to block, and do we have a pOverlapped structure?
			block && pOverlapped ?

			// Wait for the overlapped operation to complete, then return true
			WaitForSingleObject(pOverlapped->hEvent, INFINITE), true :

			// Either we're finished already, or we were not asked to block.  Just return.
			true
		) :

		// Free the pOverlapped structure, if needed
		(pOverlapped ? m_overlapPool.Free(pOverlapped), false : false);
}

eHidStatus COcuHidInstance::Zoom(double fx, double fy, double extent, double interval) const
{
	eHidStatus retVal = eHidIntrSuccess;

	// Set up the parent synthesis structure
	HID_SYNTEHSIZE_REPORT report;
	memset(&report, 0, sizeof(report));
	report.ReportID = REPORTID_SYNTHESIZE;

	// Configure the gesture report to send one (or two) fingers:
	auto& GestureReport = report.GestureReport;
	GestureReport.ReportID = REPORTID_FINGERS;
	GestureReport.ActualInputs = 1 + OCUINT_ALLOW_HYBRID_SYNTHESIS;

	// First finger, contact ID 0
	auto& finger0 = GestureReport.contacts[0];
	finger0.TipSwitch = 1;
	finger0.InRange = 1;
	finger0.bContactId = 0;

	// Possibly, the second report, if we're packing multiple touch events in one structure.
	HID_SYNTEHSIZE_REPORT report2;
	if(!OCUINT_ALLOW_HYBRID_SYNTHESIS)
		report2 = report;

	// Second finger, contact ID 1
	// We just use the original report if hybrid synthesis is allowed.  Otherwise, we have
	// to use the copy report and the first contact report therein.
	auto& finger1 = (OCUINT_ALLOW_HYBRID_SYNTHESIS ? report.GestureReport : report2.GestureReport).contacts[OCUINT_ALLOW_HYBRID_SYNTHESIS];
	finger1.TipSwitch = 1;
	finger1.InRange = 1;
	finger1.bContactId = 1;

	// Set the initial center position:
	finger0.wXData = finger1.wXData = (USHORT)(fx * FINGER_REPORT_LMAX);
	finger0.wYData = finger1.wYData = (USHORT)(fy * FINGER_REPORT_LMAX);

	// Compute the amount to move given the synthesis frequency and the computed number of steps
	USHORT moveAmount = (USHORT)(FINGER_REPORT_LMAX * extent * sc_frequency / interval);
	cout << "Move amount is " << moveAmount << endl;
	for(
		size_t iterations = (size_t)(interval / sc_frequency);
		iterations--;
		Sleep(sc_frequency)
	)
	{
		// Move by the movement amount
		finger0.wXData += moveAmount;
		finger0.wYData += moveAmount;
		finger1.wXData -= moveAmount;
		finger1.wYData -= moveAmount;

		// Any below-zero descents result in a break
		if(
			FINGER_REPORT_LMAX < finger0.wXData || FINGER_REPORT_LMAX < finger0.wYData ||
			FINGER_REPORT_LMAX < finger1.wXData || FINGER_REPORT_LMAX < finger1.wYData
		)
		{
			retVal = eHidIntrGestureOverflow;
			break;
		}

		if(
			!SendOutputReport(report) ||

			// We don't want to send off the second report if we aren't actually using it.
			(!OCUINT_ALLOW_HYBRID_SYNTHESIS && !SendOutputReport(report2))
		)
			// Terminate if we fail to send the output report
		{
			retVal = eHidIntrIoctlFailed;
			break;
		}
	}

	// Lift all contact points:
	LiftAll(2);
	return retVal;
}

eHidStatus COcuHidInstance::SendReport(BYTE bContactID, bool TipSwitch, double fx, double fy, double cx, double cy) const
{
	HID_SYNTEHSIZE_REPORT report;
	memset(&report, 0, sizeof(report));
	report.ReportID = REPORTID_SYNTHESIZE;
	report.SynthesizedReportID = REPORTID_FINGERS;

	// Pen report, synthesize a pen report
	auto& GestureReport = report.GestureReport;
	GestureReport.ActualInputs = 1;

	// Single finger synthesis:
	auto& finger = GestureReport.contacts[0];
	finger.bContactId = bContactID;
	finger.TipSwitch = TipSwitch;
	finger.InRange = 1;
	finger.wXData = (USHORT)(fx * FINGER_REPORT_LMAX);
	finger.wYData = (USHORT)(fy * FINGER_REPORT_LMAX);
	finger.cxData = (USHORT)(cx * FINGER_REPORT_LMAX);
	finger.cyData = (USHORT)(cy * FINGER_REPORT_LMAX);

	// Send the report after constructing it
	if(!SendOutputReport(report))
		return eHidIntrIoctlFailed;
	return eHidIntrSuccess;
}

eHidStatus COcuHidInstance::SendReportMouse(bool LButtonDown, bool RButtonDown, double fx, double fy) const
{
	HID_SYNTEHSIZE_REPORT report;
	memset(&report, 0, sizeof(report));
	report.ReportID = REPORTID_SYNTHESIZE;
	report.SynthesizedReportID = REPORTID_MOUSE;

	// Pen report, synthesize a pen report
	auto& MouseReport = report.MouseReport;
	MouseReport.Button1 = LButtonDown;
	MouseReport.Button2 = RButtonDown;
	MouseReport.Flags = 0;
	MouseReport.x = (USHORT)(fx * 1000);
	MouseReport.y = (USHORT)(fy * 1000);

	// Send the report after constructing it
	if(!SendOutputReport(report))
		return eHidIntrIoctlFailed;
	return eHidIntrSuccess;
}

eHidStatus COcuHidInstance::CreateIcon(COcuIcon*& pIcon)
{
	// Icon creation is a trivial task, ultimately just a constructor call on this.
	pIcon = new COcuIcon(this);
	return eHidIntrSuccess;
}
