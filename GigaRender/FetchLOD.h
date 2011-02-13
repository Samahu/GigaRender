#pragma once

#include "FetchPatchItem.h"

class FetchLOD : public FetchPatchItem
{
	unsigned int requestedLOD;

public:
	unsigned int GetRequestedLOD() const { return requestedLOD; }

	FetchLOD(Patch &target, unsigned int requestedLOD);

	void LoadToSystemMemory();
	void Decompress();
	void CopyToVideoMemory();
};