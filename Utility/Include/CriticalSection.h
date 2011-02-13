#pragma once

class CriticalSection
{
private:
	CRITICAL_SECTION criticalSection;

public:
	static CriticalSection * Create();
	~CriticalSection();

	void Enter();
	void Leave();
};