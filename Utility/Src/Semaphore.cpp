#include "stdafx.h"
#include "semaphore.h"

HANDLE Semaphore::GetHandle()
{
	return handle;
}

Semaphore * Semaphore::Create(const wchar_t * name, int maximumCount, int initialCount)
{
	Semaphore * pS = new Semaphore();
	// Create a semaphore with initial and max. counts of 10.

	pS->handle = CreateSemaphore( 
		NULL,   // default security attributes
		initialCount,   // initial count
		maximumCount,   // maximum count
		name);  // unnamed semaphore

	return pS;
}

Semaphore::~Semaphore()
{
	if (NULL != handle)
		CloseHandle(handle);
}

Semaphore * Semaphore::Create(int maximumCount, int initialCount)
{
	return Semaphore::Create(NULL, maximumCount, initialCount);
}

DWORD Semaphore::WaitForSingleObject(DWORD milliseconds)
{
	return ::WaitForSingleObject(handle, milliseconds);
}

DWORD Semaphore::WaitForSingleObject()
{
	return WaitForSingleObject(INFINITE);
}

bool Semaphore::Release(long releaseCount, long *previousCount)
{
	return 0 != ReleaseSemaphore(handle, releaseCount, previousCount);
}