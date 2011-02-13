#pragma once

class Job;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Description: A muti-threaded work items queue.
// Remarks:
// This queue has two extended properties:
// 1- it synchoronize the process of Pushing/Poping items from the queue for a multi-threaded use.
// 2- it allows the user to erase items in the middle of the queue but no inserting is allowed.
//	this is because it is based on the list data structure not on the queue, and this extended
//	property is used to cancel some out-dated work items that are no longer requested.
////////////////////////////////////////////////////////////////////////////////////////////////////
class WorkQueue
{
	typedef std::list< Job * > WORK_ITEM_QUEUE;

	WORK_ITEM_QUEUE workItemsQueue;	// a queue of work items. utilizes the list as its container.
	class CriticalSection *csQueue;	// Synchronizer

public:
	typedef WORK_ITEM_QUEUE::iterator Iterator;

	// Constructor.
	WorkQueue();
	~WorkQueue();

	bool IsEmpty() const;
	unsigned int Count() const;
	const Job & Front() const;
	Job & Front();
	const Job & Back() const;
	Job & Back();
	Iterator Push(Job *job);
	void Pop();	// removes the first work item from the queue and automatically disposes the item if the flag autoDispose is set.
	void Erase(Iterator &workItemIterator);	// automatically disposes the item if autoDispose flag is set.
	void Clear();	// clears all registered work items.
};