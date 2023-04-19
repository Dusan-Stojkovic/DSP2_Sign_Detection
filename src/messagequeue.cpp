#include "messagequeue.hpp"

//----------------------------------------------------------------------------
// MessageQueue

MessageQueue::MessageQueue()
{
	//Initialize fields
	Head = 0;
	Tail = 0;
	isEnabled = false;
	isPosted = false;
	isProcessed = false;
	isReceived = false;
}

MessageQueue::~MessageQueue()
{
}

void MessageQueue::enable(bool set)
{
	isEnabled = set;
}

bool MessageQueue::getNextMessage(Message* msg, bool waitForMessage)
{
	if ( wait == PROCESSED )
	{
		isProcessed = true;
		processed.notify_one();
		wait = NONE;
	}
	if ( waitForMessage )
	{
		sleepUntilMessage();
	}
	{
		std::unique_lock<std::mutex> lck(m);
		if ( Tail <= Head )
		{
			return false;
		}
		*msg = messages[Head & ( MAX_MESSAGES - 1 )];
		Head++;
	}
	if ( wait == RECEIVED )
	{
		isReceived = true;
		received.notify_one();
	}
	else if ( wait == PROCESSED )
	{
		wait = PROCESSED;
	}
	return true;
}

void MessageQueue::postMessage(Message* msg)
{
	if (!isEnabled)
	{
		return;
	}
	while ( Tail - Head >= MAX_MESSAGES)
	{
		usleep( 1000 );
	}
	std::unique_lock<std::mutex> lck(m);
	messages[Tail & ( MAX_MESSAGES - 1 )] = *msg;
	Tail++;
	isPosted = true;
	posted.notify_one();
	if ( wait == RECEIVED )
	{
		while (!isReceived)
		{
			received.wait(lck);
		}
	}
	else if ( wait == PROCESSED )
	{
		while (isProcessed)
		{
			processed.wait(lck);
		}
		isProcessed = false;
	}
}

void MessageQueue::sleepUntilMessage()
{
	if ( wait == PROCESSED )
	{
		isProcessed = true;
		processed.notify_one();
		wait = NONE;
	}
	std::unique_lock<std::mutex> lck(m);
	if ( Tail > Head )
	{
		return;
	}
	while (!isPosted)
	{
		posted.wait(lck);
	}
	isPosted = false;
}
