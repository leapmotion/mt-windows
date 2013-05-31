;#pragma once
;enum eHidStatus;

MessageIdTypedef=eHidStatus

SeverityNames=(
		Success			=	0x0		:		OCUHID_STATUS_SUCCESS
		Informational	=	0x1		:		OCUHID_STATUS_INFORMATION
		Warning			=	0x2		:		OCUHID_STATUS_WARNING
		Error			=	0x3		:		OCUHID_STATUS_ERROR
	)

FacilityNames=(
		HidIntr			=	0x2
	)

Messageid=0
Severity=Success
Facility=HidIntr
SymbolicName=eHidIntrSuccess
Language=English
The operation completed successfully.
.

MessageId=0
Severity=Warning
Facility=HidIntr
SymbolicName=eHidIntrGestureOverflow
Language=English
The gesture had to be aborted because one of the input contact points passed beyond the boundaries of the screen
.

MessageId=0
Severity=Error
Facility=HidIntr
SymbolicName=eHidIntrIoctlFailed
Language=English
Communication failed with the Hid emulator
.

MessageId=
SymbolicName=eHidIntrNotOcuHid
Language=English
This HID instance is a HID device, but not a Leap Motion HidEmulator
.

MessageId=
SymbolicName=eHidIntrHidAttributeQueryFail
Language=English
Failed to query the HID instance attributes on this device
.

MessageId=
SymbolicName=eHidIntrCompletionPortCreateFail
Language=English
Failed to create a completion port for asynchronous IO
.

MessageId=
SymbolicName=eHidIntrCompletionThreadNotCreated
Language=English
Failed to create a thread to wait on the completion port
.

MessageId=
SymbolicName=eHidIntrNotRespondingProperly
Language=English
The device appears to be a Leap Motion HidEmulator, but isn't responding to write reports correctly
.

MessageId=
SymbolicName=eHidIntrHbitmapCreateFail
Language=English
Failed to create an HBITMAP
.

MessageId=
SymbolicName=eHidIntrUpdateLayeredWindowFailed
Language=English
Call to UpdateLayeredWindow failed for an overlay icon
.

MessageId=
SymbolicName=eHidIntrIndexOutOfBounds
Language=English
The specified display index is out of bounds
.

MessageId=
SymbolicName=eHidIntrNoFocusWindow
Language=English
Could not identify the window that currently has focus
.

MessageId=
SymbolicName=eHidIntrPixelFormatNotRecognized
Language=English
The passed pixel format was not recognized
.

MessageId=
SymbolicName=eHidIntrCreateDibSectionFailed
Language=English
Failed to create a DIB section to hold the passed image data
.

MessageId=
SymbolicName=eHidIntrFailedToLoad
Language=English
Failed to load the specified image file.  This may be because the image doesn't exist, resource shortages
on the system, or the file itself could be corrupted or of an unsupported format.
.

MessageId=
SymbolicName=eHidIntrFailedToLock
Language=English
Failed to lock the bits of the source image.  The image file could be corrupted, or there may not be enough
system resources to lock bits.
.
