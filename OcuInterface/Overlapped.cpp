#include "StdAfx.h"
#include "Overlapped.h"

COverlapped::COverlapped(void)
{
	// The entire overlapped structure should be set to zero before
	// we proceed.  Lingering nonzero values can have strange effects
	// on consumers who may use this structure without checking it first.
	memset((LPOVERLAPPED)this, 0, sizeof(OVERLAPPED));

	// Create a default, auto-resetting, anonymous event
	hEvent = CreateEvent(nullptr, false, false, nullptr);
}

COverlapped::~COverlapped(void)
{
	// Only close the event if it has been created
	if(hEvent)
		CloseHandle(hEvent);
}

