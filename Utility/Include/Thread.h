#pragma once

class Thread
{
public:
	typedef unsigned (__stdcall * Procedure) (void *);

private:
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType;     // must be 0x1000
		LPCSTR szName;    // pointer to name (in user address space)
		DWORD dwThreadID; // thread ID (-1 = caller thread)
		DWORD dwFlags;    // reserved for future use, must be zero
	} THREADNAME_INFO;

	HANDLE handle;
	Procedure pProcedure;
	void *pParameter;
	const char* name;
	unsigned int id;
	bool started;

public:
	HANDLE GetHandle() const;
	unsigned int GetID() const;
	int GetPriority() const;
	bool SetPriority(int priority);
	Procedure GetProcedure();
	void * GetArgument() const;
	const char * GetName() const;
	bool IsAlive() const;

private:
	static unsigned __stdcall procedureHook(void *pParameter);
	static void setThreadName(LPCSTR szThreadName);

public:
	Thread(const char * name, Thread::Procedure pProcedure, void* pParameter);
	~Thread();

	void Start();
	void WaitUntilFinish();

	static HANDLE GetCurrentThreadHandle();
};