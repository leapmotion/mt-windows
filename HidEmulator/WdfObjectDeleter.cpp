#include "StdAfx.h"
#include "WdfObjectDeleter.h"

CWdfObjectDeleter::CWdfObjectDeleter(WDFOBJECT Object):
	Object(Object)
{
}

CWdfObjectDeleter::~CWdfObjectDeleter(void)
{
	if(Object)
		// Delete the object
		WdfObjectDelete(Object);
}
