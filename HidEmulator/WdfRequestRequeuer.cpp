#include "StdAfx.h"
#include "WdfRequestRequeuer.h"

CWdfRequestRequeuer::CWdfRequestRequeuer(WDFREQUEST Request, WDFQUEUE Queue):
	Request(Request),
	Queue(Queue)
{
}

CWdfRequestRequeuer::~CWdfRequestRequeuer(void)
{
	// Based on the status code (presumably of some other operation) we decide
	// what to do with our packet.  Broadly, we only want to complete the packet
	// if the status was successful.  Otherwise we put it into the queue, or if
	// the packet needs to go back to the queue it came from, we can just use
	// the requeue request.

	if(NT_SUCCESS(Status))
		// Gets completed successfully
		WdfRequestComplete(Request, Status);
	else if(Queue)
		// Goes on to a new queue
		WdfRequestForwardToIoQueue(Request, Queue);
	else
		// Goes back to where it came from.
		WdfRequestRequeue(Request);
}
