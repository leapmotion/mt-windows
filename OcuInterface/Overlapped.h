#pragma once

/// <summary>
/// This is a managed overlapped structure.  It automatically cleans up the event
/// handle associated with the OVERLAPPED structure so that it may be automatically
/// freed when no longer needed.
/// </summary>
class COverlapped:
	public OVERLAPPED
{
public:
	COverlapped(void);
	~COverlapped(void);
};

