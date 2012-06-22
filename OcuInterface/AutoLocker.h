#pragma once

/// <summary>
/// RIAA autolocker class, which locks the passed mutex on initialization,
/// and unlocks it when it leaves scope
/// </summary>
class CAutoLocker
{
public:
	/// <summary>
	/// Locks the passed mutex
	/// </summary>
	/// <param name="hLock">The lock handle to be locked.  The wait timeout is set to INFINITE.</param>
	CAutoLocker(HANDLE hLock);
	~CAutoLocker(void);

	// The handle to the lock which will be release
	HANDLE m_hLock;
};

