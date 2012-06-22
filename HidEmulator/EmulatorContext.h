#pragma once
#include "EmulatorState.h"

class CEmulator;

/// <summary>
/// Unused.  If a way was needed to track individual clients, this class would provide it.
/// Currently, it is difficult to track who has handles open to the HidEmualtor because the
/// HID class driver does not allow direct handles to be opened to the device, nor does it
/// notify the device when a handle is opened or closed.
/// </summary>
class CEmulatorContext
{
public:
	CEmulatorContext(CEmulator* pParent);
	~CEmulatorContext(void);

private:
	// The parent emulator of this context
	CEmulator* m_pParent;

	// State known to the system that is due to this emulator context:
	CEmulatorState m_state;
};

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CEmulatorContext, GetFileObjectContext)
