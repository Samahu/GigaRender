#include "stdafx.h"
#include "Worker.h"
#include "Thread.h"
#include "Semaphore.h"
#include "IBoss.h"
#include "CriticalSection.h"
#include "Job.h"

void Worker::setIdle(bool value)
{
	csIdle->Enter();
	idle = value;
	csIdle->Leave();
}

unsigned int Worker::threadProcedure(void* parameter)
{
	Worker *worker = static_cast<Worker *>(parameter);
	assert(NULL != worker);

	return worker->loop();
}

unsigned int Worker::loop()
{
	for(;;)
	{
		// notify the boss that you are going into idle state.
		if (NULL != boss)
			boss->OnWorkerGoingIdle(*this);

		if (NULL == job)	// there is no assigned job, then go on into idle state.
		{
			setIdle(true);
			assignmentControl->WaitForSingleObject();
			setIdle(false);
		}

		if (IsSuiciding())		// if suicide was activated then perform it before starting the job.
			break;

		if (NULL != job)
		{
			perform(*job);

			if (NULL != boss)
				boss->OnWorkerDone(*this, *job);

			job = NULL;	// done with this job.
		}

		if (IsSuiciding())		// before going idle check if the worker is asked to suicide.
			break;
	}

	return 0;
}

Worker::Worker(IBoss* boss, const char *name)
: boss(boss)
{
	thread = new Thread(name, threadProcedure, this);
	assignmentControl = Semaphore::Create(1, 0);	// start stopped.
	suicide = false;
	job = NULL;

	csIdle = CriticalSection::Create();
	csSuicide = CriticalSection::Create();
	idle = true;
}

Worker::~Worker()
{
	CommitSuicide();

	if (NULL != assignmentControl)
		delete assignmentControl;

	if (NULL != thread)
		delete thread;

	if (NULL != csIdle)
		delete csIdle;

	if (NULL != csSuicide)
		delete csSuicide;
}

void Worker::Start()
{
	assert(NULL != thread);
	thread->Start();
}

void Worker::CommitSuicide()
{
	if (IsSuiciding())	// suicide commiting is in progress.
		return;

	setSuicide(true);

	if (IsIdle())	// wake up the worker so he can commit his last special assignment.
		assignmentControl->Release();
}

bool Worker::IsSuiciding() const
{
	bool copy;
	csSuicide->Enter();
	copy = suicide;
	csSuicide->Leave();
	return copy;
}

void Worker::setSuicide(bool value)
{
	csSuicide->Enter();
	suicide = value;
	csSuicide->Leave();
}

bool Worker::IsIdle() const
{
	bool result;
	csIdle->Enter();
	result = idle;
	csIdle->Leave();
	return result;
}

void Worker::QuitIdle()
{
	assignmentControl->Release();
}

bool Worker::Assign(Job &job)
{
	if (NULL != this->job || IsSuiciding())	// if there is already an assigned work item, then assigning another one isn't acceptable.
		return false;

	this->job = &job;
	assert(this->job != NULL);
	QuitIdle();

	return true;
}