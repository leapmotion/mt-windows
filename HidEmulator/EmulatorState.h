#pragma once

/// <summary>
/// Kernel-mode state tracking structure of the HID emulator.
/// </summary>
/// <remarks>
/// In the event that a client goes away, it may be necessary to reset the state of the HID emualtor.
/// In that case, up-events are sent to the operating system for each contact point currently down
/// on the screen, thereby freeing any "stuck mouse" situation that may arise if an application crashes
/// while synthesizing a touch event.
///
/// Note also that only those states that are due to a particular client are reset.  No touch events
/// are synthesized to an "up" state if a client crashes, provided that client is not currently the cause
/// of a finger press.
/// </remarks>
class CEmulatorState
{
public:
	CEmulatorState(void);
	~CEmulatorState(void);

public:
	// Current state of each fingertip.  This is defined as the last input synthesized on the interface.
	HID_FINGER_REPORT touchState[16];

	// Current mouse state, as synthesized by the client.
	HID_MOUSE_REPORT mouseState;
};

