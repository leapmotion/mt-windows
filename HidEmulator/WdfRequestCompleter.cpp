#include "StdAfx.h"
#include "WdfRequestCompleter.h"

CWdfRequestCompleter::CWdfRequestCompleter(WDFREQUEST Request):
	Request(Request),
	Status(STATUS_SUCCESS)
{
}

CWdfRequestCompleter::~CWdfRequestCompleter(void)
{
	// Complete the packet unconditionally based on the current status value.
	WdfRequestComplete(Request, Status);
}
