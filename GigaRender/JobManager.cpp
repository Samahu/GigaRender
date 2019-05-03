#include "stdafx.h"
#include "JobManager.h"
#include "WorkPool.h"
#include "WorkQueue.h"
#include "Patch.h"
#include "FetchLOD.h"
#include "FetchTexture.h"
#include "PatchItemLoader.h"
#include "PatchItemDecompressor.h"

#define DISABLE_PENDING_REQUESTS_REGISTER

unsigned int JobManager::GetTotalLODsCount() const
{
	return totalLODsCount;
}

JobManager::JobManager(unsigned int totalLODsCount, bool utilizeTexture)
: totalLODsCount(totalLODsCount)
{
	unsigned int queuesCount = 0;
	queuesCount += totalLODsCount;	// construct a separate work queues for each lod.
	if (utilizeTexture)
		++queuesCount;				// construct a separate work queue for texture requests

	loadJobs = WorkPool::Create(queuesCount);
	decompressJobs = WorkPool::Create(1);
	copyJobs = WorkPool::Create(1);

	loader = new PatchItemLoader(this, "PatchItemLoader");
	loader->Start();

	decompressor = new PatchItemDecompressor(this, "PatchItemDecompressor");
	decompressor->Start();
}

JobManager::~JobManager()
{
	if (NULL != loader)
		delete loader;

	if (NULL != decompressor)
		delete decompressor;

	Job *job;
	for (unsigned int i = 0; i < loadJobs->GetWorkQueuesCount(); ++i)
	{
		WorkQueue &workQueue = loadJobs->GetWorkQueue(i);

		while(!workQueue.IsEmpty())
		{
			job = &workQueue.Front();
			workQueue.Pop();
			delete job;
		}
	}

	for (unsigned int i = 0; i < decompressJobs->GetWorkQueuesCount(); ++i)
	{
		WorkQueue &workQueue = decompressJobs->GetWorkQueue(i);

		while(!workQueue.IsEmpty())
		{
			job = &workQueue.Front();
			workQueue.Pop();
			delete job;
		}
	}

	for (unsigned int i = 0; i < copyJobs->GetWorkQueuesCount(); ++i)
	{
		WorkQueue &workQueue = copyJobs->GetWorkQueue(i);

		while(!workQueue.IsEmpty())
		{
			job = &workQueue.Front();
			workQueue.Pop();
			delete job;
		}
	}

	if (NULL != loadJobs)
		delete loadJobs;

	if (NULL != decompressJobs)
		delete decompressJobs;

	if (NULL != copyJobs)
		delete copyJobs;
}

void JobManager::RequestFillPatch(Patch &target, unsigned int lodsToFillCount, bool fillTexture)
{
	if(lodsToFillCount > totalLODsCount)
		lodsToFillCount = totalLODsCount;

	// use the first work queues to pend request of each lod ascendingly.
	unsigned int i;
	WorkQueue::Iterator iterator;
	for (i = 0; i < lodsToFillCount; ++i)
	{
		if (i > 2)
			break;

		FetchLOD *fetchLOD = new FetchLOD(target, i);
		assert(NULL != fetchLOD);
		iterator = loadJobs->GetWorkQueue(i).Push(fetchLOD);

#ifndef DISABLE_PENDING_REQUESTS_REGISTER
		target.PendRequest(new RequestID(i, iterator));
#endif
	}

	// use the last queue to pend the requests of texture loading.
	if (fillTexture)
	{
		FetchTexture *fetchTexture = new FetchTexture(target);
		assert(NULL != fetchTexture);
		iterator = loadJobs->GetWorkQueue(i).Push(fetchTexture);
	}

#ifndef DISABLE_PENDING_REQUESTS_REGISTER
	target.PendRequest(new RequestID(i, iterator));
#endif

	// force the loader to quit its idle state.
	loader->QuitIdle();
}

void JobManager::RequestFillPatch(Patch &target, bool fillTexture)
{
	RequestFillPatch(target, totalLODsCount, fillTexture);
}

void JobManager::CancelFillPatchRequest(Patch &target)
{
#ifndef DISABLE_PENDING_REQUESTS_REGISTER
	std::list<RequestID *> &requests = target.LockPendingRequests();

	std::list<RequestID *>::iterator i, e = requests.end();
	for (i = requests.begin(); i != e;)
	{
		RequestID *request = *i;
		workPool->GetWorkQueue(request->GetQueueIndex()).Erase(request->GetIterator());
		delete request;
		i = requests.erase(i);
	}

	target.UnlockPendingRequests();
#endif
}

void JobManager::OnWorkerDone(Worker &worker, Job &job)
{
	if (&worker == loader)	// loader finished loading some stuff.
	{
		decompressJobs->GetWorkQueue(0).Push(&job);
		decompressor->QuitIdle();
	}
	else if(&worker == decompressor)
	{
		copyJobs->GetWorkQueue(0).Push(&job);
	}
	else
	{
		assert(false);	// what the f...
	}
}

void JobManager::OnWorkerGoingIdle(Worker &worker)
{
	if (&worker == loader)
		loaderGoingIdle();
	else if (&worker == decompressor)
		decompressorGoingIdle();
	else
		assert(false);	// what the f...
}

void JobManager::loaderGoingIdle()
{
	// go to the first queue that have work to be done
	for (unsigned int i = 0; i < loadJobs->GetWorkQueuesCount(); ++i)
	{
		WorkQueue & workQueue = loadJobs->GetWorkQueue(i);

		if (workQueue.IsEmpty())
			continue;

		Job &job = workQueue.Front();
FetchPatchItem &fpi =
#if defined(DEBUG)
			dynamic_cast< FetchPatchItem & >(job);
#else
			static_cast< FetchPatchItem & >(job);
#endif

		Patch &target = fpi.GetTarget();
		workQueue.Pop();
		target.IncrementSubmittedRequestsCount();
		loader->Assign(job);
		return;
	}
}

void JobManager::decompressorGoingIdle()
{
	WorkQueue &workQueue = decompressJobs->GetWorkQueue(0);

	if(workQueue.IsEmpty())
		return;					// nothing to be assigned.

	Job &job = workQueue.Front();
	workQueue.Pop();
	decompressor->Assign(job);
}

void JobManager::CopyItems(unsigned int itemsCount)
{
	WorkQueue &workQueue = copyJobs->GetWorkQueue(0);

	while (!workQueue.IsEmpty() && itemsCount-- > 0)
	{
		Job &job = workQueue.Front();
		workQueue.Pop();
		
		FetchPatchItem &fpi =
#if defined(DEBUG)
			dynamic_cast< FetchPatchItem & >(job);
#else
			static_cast< FetchPatchItem & >(job);
#endif

		fpi.CopyToVideoMemory();

		fpi.GetTarget().DecrementSubmittedRequestCount();
		delete &job;
	}
}

/*
void JobManager::Process()
{
	if (worker->IsIdle())
	{
		// go to the first queue that have work to be done
		for (unsigned int i = 0; i < workPool->GetWorkQueuesCount(); ++i)
		{
			WorkQueue & workQueue = workPool->GetWorkQueue(i);

			if (workQueue.IsEmpty())
				continue;

			Job & job = workQueue.Front();
			Patch &target = job.GetTarget();

#ifndef DISABLE_PENDING_REQUESTS_REGISTER
			// remove the work item from the list
			std::list<RequestID *> &requests = target.LockPendingRequests();
			std::list<RequestID *>::iterator j, e = requests.end();
			for (j = requests.begin(); j != e;)
			{
				RequestID *request = *j;

				WorkQueue::Iterator &req_iter = request->GetIterator();
				Job *req_workItem = *req_iter;

				if (request->GetQueueIndex() == i &&
					req_workItem->GetID() == job.GetID())
				{
					delete request;
					requests.erase(j);
					break;
				}
			}
			target.UnlockPendingRequests();
#endif

			workQueue.Pop();

			target.IncrementSubmittedRequestsCount();

			worker->Assign(job);

			return;
		}
	}
}
*/