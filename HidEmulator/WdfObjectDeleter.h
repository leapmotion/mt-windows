#pragma once

/// <summary>
/// RAII object deleter
/// </summary>
class CWdfObjectDeleter
{
public:
	/// <summary>
	/// Deletes the passed object if it's still attached when this deleter is destroyed
	/// </summary>
	/// <param name="Object">The object to be deleted</param>
	CWdfObjectDeleter(WDFOBJECT Object);
	~CWdfObjectDeleter(void);

private:
	// The object to be deleted
	WDFOBJECT Object;

public:
	/// <summary>
	/// Prevents this deleter from deleting the underlying request
	/// </summary>
	void Steal(void)
	{
		Object = nullptr;
	}
};

