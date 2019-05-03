#include "stdafx.h"
#include "WorkQueue.h"
#include "Job.h"
#include "CriticalSection.h"

WorkQueue::WorkQueue()
{
	csQueue = CriticalSection::Create();
}

WorkQueue::~WorkQueue()
{
	// make sure that the work queue is empty.
	Clear();

	if(NULL != csQueue)
		delete csQueue;
}

bool WorkQueue::IsEmpty() const
{
	bool isEmpty;
	csQueue->Enter();
	isEmpty = workItemsQueue.empty();
	csQueue->Leave();
	return isEmpty;
}

unsigned int WorkQueue::Count() const
{
	unsigned int count;
	csQueue->Enter();
	count = (unsigned int)workItemsQueue.size();
	csQueue->Leave();
	return count;
}

const Job & WorkQueue::Front() const
{
	Job *job;
	csQueue->Enter();
	job = workItemsQueue.front();
	csQueue->Leave();
	return *job;
}

Job & WorkQueue::Front()
{
	Job *job;
	csQueue->Enter();
	job = workItemsQueue.front();
	csQueue->Leave();
	return *job;
}

const Job & WorkQueue::Back() const
{
	Job *job;
	csQueue->Enter();
	job = workItemsQueue.back();
	csQueue->Leave();
	return *job;
}

Job & WorkQueue::Back()
{
	Job *job;
	csQueue->Enter();
	job = workItemsQueue.back();
	csQueue->Leave();
	return *job;
}

WorkQueue::Iterator WorkQueue::Push(Job *job)
{
	WorkQueue::Iterator *iterator;
	csQueue->Enter();
	workItemsQueue.push_back(job);
	iterator = &(--workItemsQueue.end());
	csQueue->Leave();
	return *iterator;
}

void WorkQueue::Pop()
{
	csQueue->Enter();
	workItemsQueue.pop_front();
	csQueue->Leave();
}

void WorkQueue::Erase(WorkQueue::Iterator &workItemIterator)
{
	csQueue->Enter();
	workItemsQueue.erase(workItemIterator);
	csQueue->Leave();
}

void WorkQueue::Clear()
{
	csQueue->Enter();
	workItemsQueue.clear();
	csQueue->Leave();
}