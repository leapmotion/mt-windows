#include "StdAfx.h"
#include "WdfObjectLock.h"

CWdfObjectLock::CWdfObjectLock(WDFOBJECT obj):
	m_obj(obj)
{
	// Acquire a lock on the object
	WdfObjectAcquireLock(m_obj);
}

CWdfObjectLock::~CWdfObjectLock(void)
{
	// Logical inverse:  Release the already-acquired lock
	WdfObjectReleaseLock(m_obj);
}
