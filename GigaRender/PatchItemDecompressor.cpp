#include "stdafx.h"
#include "PatchItemDecompressor.h"
#include "FetchPatchItem.h"

void PatchItemDecompressor::perform(Job &job)	// override perform
{
	FetchPatchItem &fpi =
#if defined(DEBUG)
		dynamic_cast< FetchPatchItem & >(job);
#else
		static_cast< FetchPatchItem & >(job);
#endif

	fpi.Decompress();
}

PatchItemDecompressor::PatchItemDecompressor(IBoss *boss, const char* name)
: Worker(boss, name)
{
}