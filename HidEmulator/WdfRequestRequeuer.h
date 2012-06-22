#pragma once

/// <summary>
/// Enables automatic requeuing of a request to the specified queue under certain conditions.
/// </summary>
/// <remarks>
/// NT_STATUS(Success) -> Complete <c>Request</c> with a status value of Success
/// otherwise -> Return <c>Request</c> to the specified queue, based on queueing behavior
/// </remarks>
class CWdfRequestRequeuer
{
public:
	/// <param name="Request">The request to be managed</param>
	/// <param name="Queue">
	/// The queue to which <paramref name="Request"/> should be returned, or nullptr if WdfRequestRequeue
	/// should be called instead
	/// </param>
	CWdfRequestRequeuer(WDFREQUEST Request, WDFQUEUE Queue = nullptr);
	~CWdfRequestRequeuer(void);

private:
	// This is the request that will be queued
	WDFREQUEST Request;

	// This is the queue to which the request will be queued
	WDFQUEUE Queue;

public:
	NTSTATUS Status;
};

