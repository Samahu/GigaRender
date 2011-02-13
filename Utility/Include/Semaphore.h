#pragma once

class Semaphore
{
private:
	HANDLE handle;

public:
	HANDLE GetHandle();

public:
	static Semaphore * Create(const wchar_t * name, int maximumCount, int initialCount);
	static Semaphore * Create(int maximumCount, int initialCount);
	~Semaphore();

	DWORD WaitForSingleObject(DWORD milliseconds);
	DWORD WaitForSingleObject();
	bool Release(long releaseCount = 1, long *previousCount = NULL);
};