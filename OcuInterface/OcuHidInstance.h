#pragma once
#include "ObjPool.h"
#include "OcuInterfaceCodes.h"
#include "Overlapped.h"
#include "../Globals/Interface.h"
#include <map>

using std::map;

class COcuIcon;
class COcuImage;

/// <summary>
/// Represents a single instance of a HID emulator
/// </summary>
/// <remarks>
/// A single instance of a HID emulator is represented by the system as a single logical device.  Such a
/// device may be independently calibrated or configured to render on a particular monitor using multidigimon.
/// The instance may also potentially be invalid.  Instances of this class should not be used unless the call
/// to Initialize succeeds.
/// </remarks>
class COcuHidInstance
{
public:
	/// <summary>
	/// Creates a HID instance with a given device and asynchronous state
	/// </summary>
	/// <param name="hDevice">A handle to the device to be created</param>
	/// <param name="bAsynchronous">Set if the created instance should operate in asynchronous mode</param>
	/// <remarks>
	/// In asynchronous operation, the passed handle must have been opened with FILE_FLAG_OVERLAPPED.  Afterwards,
	/// any IO operations invoked on this instance will complete immediately.  The underlying implementation makes
	/// use of IO completion ports, which result in the creation of a separate worker thread to handle dispatching
	/// of pended OVERLAPPED instances.
	/// </remarks>
	COcuHidInstance(HANDLE hDevice, bool bAsynchronous);
	~COcuHidInstance(void);

	/// <summary>
	/// This is the frequency with which input reports are synthesized for metaoperations, in milliseconds
	/// </summary>
	static const DWORD sc_frequency = 100;

	/// <summary>
	/// This is the initialization operation.  Call this before attempting to operate on a HID instance
	/// </summary>
	eHidStatus Initialize(void);

private:
	// Device handle itself:
	HANDLE m_hDevice;

	// True if this device is asynchronous
	bool m_bAsynchronous;

	// Completion port used with asynchronous IO
	HANDLE m_hCompletionPort;

	// Thread properties of the thread used with the completion port:
	HANDLE m_hCompletionThread;
	DWORD m_dwCompletionTid;

	// OVERLAPPED structures used in pending operations:
	HANDLE m_hLock;
	mutable CObjPool<COverlapped> m_overlapPool;

	// True if this is an ocuhid instance:
	bool m_bIsOcuHid;

private:
	/// <summary>
	/// Completion thread main entrypoint, for use during asynchronous operation
	/// </summary>
	static DWORD __stdcall CompletionThread(void* pObj)
	{
		return ((COcuHidInstance*)pObj)->DoCompletionThread();
	}

	/// <summary>
	/// Completion thread main entrypoint
	/// </summary>
	DWORD DoCompletionThread(void);

public:
	/// <summary>
	/// Safely sends an IOCTL_HID_SET_OUTPUT_REPORT
	/// </summary>
	/// <returns>
	/// The result of the DeviceIoControl call
	/// </returns>
	/// <remarks>
	/// Asynchronous IO prevents us from using HidD_SetOutputReport, as this
	/// routine assumes that the handle will be synchronous.
	/// </remarks>
	bool SendOutputReport(const HID_SYNTEHSIZE_REPORT& report, bool block = false) const;

	/// <summary>
	/// Synthesizes a number of "contact lifted" messages
	/// </summary>
	/// <param name="nPoints">The number of points to be lifted</param>
	void LiftAll(UCHAR nPoints) const;

public:
	// Accessor methods:
	HANDLE GetHandle(void) const {return m_hDevice;}
	DWORD GetVersion(void) const;

	/// <return>
	/// True if the parent HID instance is an Leap Motion HID instance
	/// </return>
	bool IsOcuHid(void) const {return m_bIsOcuHid;}
	
	/// <summary>
	/// Creates a new icon with the specified touch identifier
	/// </summary>
	eHidStatus CreateIcon(COcuIcon*& pIcon);

	/// <summary>
	/// Emulates a zoom gesture at the specified screen offsets:
	/// </summary>
	/// <param name="fx">The proportionally centered X coordinate</param>
	/// <param name="fy">The proportionally centered Y coordinate</param>
	/// <param name="extent">The total distance to move</param>
	/// <param name="interval">Total time to complete the gesture, in milliseconds</param>
	eHidStatus Zoom(double fx, double fy, double extent, double interval) const;

	/// <summary>
	/// Sends an arbitrary forged report down to the lower-level driver
	/// </summary>
	/// <param name="bContactID">The index of the finger on which the report is to be sent</param>
	/// <param name="TipSwitch">
	/// Set to true to turn on the tip switch.  The tip switch is translated to a left mouse click by the HID stack.
	/// </param>
	/// <param name="fx">The x coordinate of the cursor</param>
	/// <param name="fy">The y coordinate of the cursor</param>
	/// <param name="cx">The width of the contact pad for the cursor</param>
	/// <param name="cy">The height of the contact pad for the cursor</param>
	eHidStatus SendReport(BYTE bContactID, bool TipSwitch, double fx, double fy, double cx = 0.0, double cy = 0.0) const;

	/// <summary>
	/// Sends a mouse event with the specified parameters
	/// </summary>
	eHidStatus SendReportMouse(bool LButtonDown, bool RButtonDown, double fx, double fy) const;
};

