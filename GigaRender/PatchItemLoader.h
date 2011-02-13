#pragma once

#include "Worker.h"

class PatchItemLoader : public Worker
{
private:
	void perform(Job &job);	// override perform

public:
	PatchItemLoader(IBoss *boss, const char* name);
};