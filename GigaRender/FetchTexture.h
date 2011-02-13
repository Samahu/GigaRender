#pragma once

#include "FetchPatchItem.h"

class FetchTexture : public FetchPatchItem
{
public:
	FetchTexture(Patch &target);

	virtual void LoadToSystemMemory();	// Load from hard disk to system memory.
	virtual void Decompress();			// Decompress.
	virtual void CopyToVideoMemory();	// copy decompressed version from system memory to video memory.
};