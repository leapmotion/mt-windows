#pragma once
#include "FocusAppInfo.h"
#include "OcuHidInstance.h"
#include <vector>

using std::vector;

class COcuHidInstance;
class COcuIcon;
class COcuImage;

class COcuInterface:
	public vector<COcuHidInstance*>
{
public:
	/// <summary>
	/// Creates a new interface for enumerating HID instances
	/// </summary>
	/// <param name="bAsynchronous">Set to true if calls to input synthesis should be performed asynchronously</param>
	/// <remarks>
	/// Asynchronous IO is supported in OcuHID by using I/O completion ports and an additional dispatcher thread to handle
	/// resource cleanup.
	/// </remarks>
	COcuInterface(bool bAsynchronous = false);
	~COcuInterface(void);

private:
	DWORD m_lastErr;
	bool m_bAsynchronous;

public:
	bool Ok(void) const {return !m_lastErr;}
	DWORD GetLastErr(void) const {return m_lastErr;}

	/// <summary>
	/// Removes the first compliant COcuHidInstance from this collection and returns it.  The caller is responsible
	/// for deleting the returned pointer.
	/// </summary>
	COcuHidInstance* GetFirstCompliantInterface(void);
};

