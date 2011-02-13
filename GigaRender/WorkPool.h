#pragma once

class WorkQueue;

///////////////////////////////////////////////////////////////////////////////
// Class Name: WorkPool
// Description: A class that serves as a containter for work queues and also
//  manages their construction and destruction.
///////////////////////////////////////////////////////////////////////////////
class WorkPool
{
private:
	unsigned int workQueuesCount;
	WorkQueue** workQueues;

	// Constructor
	WorkPool(unsigned int workQueuesCount, WorkQueue *workQueues[]);

public:
	static WorkPool* Create(unsigned int workQueuesCount);
	// Destructor
	~WorkPool();

	WorkQueue & GetWorkQueue(unsigned int index);
	unsigned int GetWorkQueuesCount() const;
};