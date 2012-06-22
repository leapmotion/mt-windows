#pragma once

/// <summary>
/// Enables a request to be completed automatically at scope exit
/// </summary>
class CWdfRequestCompleter
{
public:
	CWdfRequestCompleter(WDFREQUEST Request);
	~CWdfRequestCompleter(void);

	WDFREQUEST Request;
	NTSTATUS Status;

	/// <summary>
	/// Sets the status that the attached request should be completed with
	/// when this instance is destroyed
	/// </summary>
	/// <param name="status">The status to be assigned</param>
	void SetStatus(NTSTATUS status) {Status = status;}
};

