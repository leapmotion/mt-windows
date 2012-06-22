#include "StdAfx.h"
#include "AutoLocker.h"

CAutoLocker::CAutoLocker(HANDLE hLock):
	m_hLock(hLock)
{
	// Acquire the passed lock
	WaitForSingleObject(m_hLock, INFINITE);
}

CAutoLocker::~CAutoLocker(void)
{
	// Release the mutex.  This makes the assumption that the mutex was acquired,
	// which is typically a safe assumption unless the application is shutting down.
	ReleaseMutex(m_hLock);
}
