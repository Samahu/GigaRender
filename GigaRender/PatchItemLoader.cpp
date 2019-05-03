#include "stdafx.h"
#include "PatchItemLoader.h"
#include "FetchPatchItem.h"

void PatchItemLoader::perform(Job &job)	// override perform
{
	FetchPatchItem &fpi =
#if defined(DEBUG)
		dynamic_cast< FetchPatchItem & >(job);
#else
		static_cast< FetchPatchItem & >(job);
#endif

	fpi.LoadToSystemMemory();
}

PatchItemLoader::PatchItemLoader(IBoss *boss, const char* name)
: Worker(boss, name)
{
}