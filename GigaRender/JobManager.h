#pragma once

#include "IBoss.h"

class WorkPool;
class Patch;
class PatchItemLoader;
class PatchItemDecompressor;


class JobManager : public IBoss
{
private:
	unsigned int totalLODsCount;
	WorkPool* loadJobs;
	WorkPool* decompressJobs;
	WorkPool* copyJobs;

	PatchItemLoader *loader;
	PatchItemDecompressor *decompressor;


	JobManager & operator= (JobManager &) {}	// removes warning: [assignment operator could not be generated].

private:
	// Implementation of interface IBoss.
	void OnWorkerDone(Worker &worker, Job &job);
	void OnWorkerGoingIdle(Worker &worker);

	void loaderGoingIdle();
	void decompressorGoingIdle();

public:
	unsigned int GetTotalLODsCount() const;
	JobManager(unsigned int totalLODsCount, bool utilizeTexture = true);
	~JobManager();

	// if lodsToFillCount is larger than the specified total lods it will get clipped.
	void RequestFillPatch(Patch &target, unsigned int lodsToFillCount, bool fillTexture = true);
	// tries to fill to most available lod.
	void RequestFillPatch(Patch &target, bool fillTexture = true);
	void CancelFillPatchRequest(Patch &target);

	// itemsCount: maximum count of items to be copied per call.
	void CopyItems(unsigned int itemsCount = 1);
};