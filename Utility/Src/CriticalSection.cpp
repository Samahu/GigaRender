#include "stdafx.h"
#include "CriticalSection.h"

CriticalSection * CriticalSection::Create()
{
	CriticalSection *pCS = new CriticalSection();

	InitializeCriticalSection(&pCS->criticalSection);

	return pCS;
}

void CriticalSection::Enter()
{
	EnterCriticalSection(&criticalSection);
}

void CriticalSection::Leave()
{
	LeaveCriticalSection(&criticalSection);
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&criticalSection);
}