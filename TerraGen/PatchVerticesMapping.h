#pragma once

class PatchResolution;

// A class that maps vertices indices with respect to their use in a given level of detail
class PatchVerticesMapping
{
private:
	PatchResolution &patchResolution;
	int **pData;						// holds the each vertex mapping
	int *pLevelVerticesCount;			// holds the accumulated level vertices count

public:
	const PatchResolution & GetPatchResolution() const;
	int GetVerticesCountPerRow() const;
	int GetVerticesCountPerPatch() const;
	int GetAccumulatedLevelVerticesCount(int level) const;
	int GetNonAccumulatedLevelVerticesCount(int level) const;

private:
	// Private Constructor
	PatchVerticesMapping(PatchResolution &patchResolution);

	PatchVerticesMapping& operator= (const PatchVerticesMapping&) {} // removes warning: assignment operator couldn't be generated

public:
	// Constructor Guard
	static PatchVerticesMapping* Create(PatchResolution *pPatchResolution);
	// Destructor
	~PatchVerticesMapping();

	int operator()(int row, int col) const;
};