#include "stdafx.h"
#include "Thread.h"

HANDLE Thread::GetHandle() const
{
	return handle;
}

unsigned int Thread::GetID() const
{
	return id;
}

int Thread::GetPriority() const
{
	return GetThreadPriority(handle);
}

bool Thread::SetPriority(int priority)
{
	return 0 != SetThreadPriority(handle, priority);
}

bool Thread::IsAlive() const
{
	DWORD exitCode;
	GetExitCodeThread(handle, &exitCode);
	return STILL_ACTIVE == exitCode;
}

unsigned int Thread::procedureHook(void *pParameter)
{
	Thread *pThread = static_cast<Thread *>(pParameter);
	assert(NULL != pThread);
	if (!pThread->started)
	{
		pThread->setThreadName(pThread->name);
		pThread->started = true;
	}

	return pThread->pProcedure(pThread->pParameter);
}

void Thread::setThreadName(LPCSTR szThreadName)
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = szThreadName;
    info.dwThreadID = (DWORD)-1;
    info.dwFlags = 0;

    __try
    {
        RaiseException( 0x406D1388, 0, sizeof(info) / sizeof(DWORD), (DWORD*)&info );
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
	{
    }
}

Thread::Thread(const char * name, Thread::Procedure pProcedure, void *pParameter)
: name(name), pProcedure(pProcedure), pParameter(pParameter)
{
	started = false;
	handle = (HANDLE)_beginthreadex(
		NULL,
		0,
		procedureHook,
		this,
		CREATE_SUSPENDED,
		&id);
}

Thread::~Thread()
{
	WaitUntilFinish();

#if defined(DEBUG) || defined(_DEBUG)
	assert(TRUE == CloseHandle(handle));
#else
	CloseHandle(handle);
#endif
}

void Thread::Start()
{
	ResumeThread(handle);
}

void Thread::WaitUntilFinish()
{
	WaitForSingleObject(handle, INFINITE);
}

HANDLE Thread::GetCurrentThreadHandle()
{
	return GetCurrentThread();
}