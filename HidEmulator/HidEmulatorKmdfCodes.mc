;#pragma once
;//
;//  Status values are 32 bit values layed out as follows:
;//
;//   3   2                   1                   0
;//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
;//  +---+-+-------------------------+-------------------------------+
;//  |Sev|C|       Facility          |               Code            |
;//  +---+-+-------------------------+-------------------------------+
;//
;//  where
;//
;//      Sev - is the severity code
;//
;//          00 - Success
;//          01 - Informational
;//          10 - Warning
;//          11 - Error
;//
;//      C - is the Customer code flag
;//
;//      Facility - is the facility code
;//
;//      Code - is the facility's status code
;//

MessageIdTypedef=eHidStatus

SeverityNames=(
		Success			=	0x0		:		OCUHID_STATUS_SUCCESS
		Informational	=	0x1		:		OCUHID_STATUS_INFORMATION
		Warning			=	0x2		:		OCUHID_STATUS_WARNING
		Error			=	0x3		:		OCUHID_STATUS_ERROR
	)

FacilityNames=(
		HidEm			=	0x0
	)

MessageId=0
Severity=Error
Facility=HidEm
SymbolicName=eHidInstFailure
Language=English
General failure condition
.
