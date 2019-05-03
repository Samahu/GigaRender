#include "stdafx.h"
#include "WorkPool.h"
#include "WorkQueue.h"

WorkPool::WorkPool(unsigned int workQueuesCount, WorkQueue *workQueues[])
: workQueuesCount(workQueuesCount), workQueues(workQueues)
{
}

WorkPool * WorkPool::Create(unsigned int workQueuesCount)
{
	if (!(workQueuesCount > 0))
		return NULL;

	WorkQueue** queues = new WorkQueue*[workQueuesCount];
	for (unsigned int i = 0; i < workQueuesCount; ++i)
		queues[i] = new WorkQueue;

	return new WorkPool(workQueuesCount, queues);
}

WorkPool::~WorkPool()
{
	if (NULL != workQueues)
	{
		for (unsigned int i = 0; i < workQueuesCount; ++i)
			if (NULL != workQueues[i])
				delete workQueues[i];

		delete [] workQueues;
	}
}

WorkQueue & WorkPool::GetWorkQueue(unsigned int index)
{
	assert(index < workQueuesCount);
	return *workQueues[index];
}

unsigned int WorkPool::GetWorkQueuesCount() const
{
	return workQueuesCount;
}