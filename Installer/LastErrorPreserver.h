#pragma once

/// <summary>
/// This ensures that the last error recovered in the constructor call
/// is preserved when the function exit.  This is imporant for functions
/// that must preserve error information, but must also run cleanup logic
/// that may change the thread error code.
/// </summary>
class CLastErrorPreserver
{
public:
	~CLastErrorPreserver(void);

private:
	DWORD m_lastError;

public:
	void CaptureLastError(void)
	{
		m_lastError = GetLastError();
	}
};

