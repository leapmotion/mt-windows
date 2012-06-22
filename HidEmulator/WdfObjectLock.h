#pragma once

/// <summary>
/// RAII-style object lock
/// </summary>
class CWdfObjectLock
{
public:
	/// <summary>
	/// Locks the passed object, and saves it to be unlocked on destruction
	/// </summary>
	/// <param name="obj">The object to be locked</param>
	CWdfObjectLock(WDFOBJECT obj);
	~CWdfObjectLock(void);

	WDFOBJECT m_obj;
};

