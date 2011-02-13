#pragma once

#include "Job.h"

class FetchPatchItem : public Job
{
protected:
	Patch &target;

public:
	Patch & GetTarget() { return target; }

	FetchPatchItem(Patch &target);

	virtual void LoadToSystemMemory() = 0;	// Load from hard disk to system memory.
	virtual void Decompress() = 0;			// Decompress.
	virtual void CopyToVideoMemory() = 0;	// copy decompressed version from system memory to video memory.
};