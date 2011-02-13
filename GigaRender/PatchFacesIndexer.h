#pragma once

class PatchResolution;
class PatchVerticesMapping;

class PatchFacesIndexer
{
private:
	PatchResolution& patchResolution;
	int *pLevelVisibleFacesCount;
	int *pLevelInvisibleFacesCount;
	int *pLevelStartIndex;

public:
	int GetLevelVisibleFacesCount(int level) const;
	int GetLevelInvisibleFacesCount(int level) const;
	int GetLevelStartIndex(int level) const;

private:
	// Private Constructor
	PatchFacesIndexer(PatchResolution &patchResolution,
		int *pLVisFaceCount, int *pLInvFaceCount, int *pLStartIndex);

	PatchFacesIndexer& operator= (const PatchFacesIndexer&) {} // removes warning: assignment operator couldn't be generated

public:
	// Constructor Guard
	static PatchFacesIndexer* Create(PatchResolution* pPatchResolution);
	// Destuctor
	~PatchFacesIndexer();

	int LevelFacesCount(int level) const;	// the sum of visible & invisible faces count of a given level
	int FacesOfAllLevelsCount() const;		// sums each level faces count with the next level faces uptill the max level
	int LevelIndicesCount(int level) const;	//
	int IndicesOfAllLevelsCount() const;	//
	int LevelStartIndex(int level) const;

	// @param pBuffer: a buffer to hold the generated indices
	void GenerateIndices(WORD *pBuffer, const PatchVerticesMapping &patchVerticesMapping) const;
};