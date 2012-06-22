#include "stdafx.h"

// Standard destructor.  Not used, and so, not implemented.  Should assert in debug mode.
void __cdecl operator delete(void* pMemory)
{
	ASSERT(false);
}