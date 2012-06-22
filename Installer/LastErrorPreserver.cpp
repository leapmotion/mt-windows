#include "StdAfx.h"
#include "LastErrorPreserver.h"

CLastErrorPreserver::~CLastErrorPreserver(void)
{
	// Restore the last error code
	SetLastError(m_lastError);
}
