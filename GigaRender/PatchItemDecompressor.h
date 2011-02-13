#pragma once

#include "Worker.h"

class PatchItemDecompressor : public Worker
{
private:
	void perform(Job &job);	// override perform

public:
	PatchItemDecompressor(IBoss *boss, const char* name);
};