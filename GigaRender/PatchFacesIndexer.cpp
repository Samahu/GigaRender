#include "dxstdafx.h"
#include "PatchFacesIndexer.h"
#include "PatchResolution.h"
#include "PatchVerticesMapping.h"

int PatchFacesIndexer::GetLevelVisibleFacesCount(int i) const
{
	if (NULL == pLevelVisibleFacesCount)
		return -1;

	if (i < 0 || i >= patchResolution.GetLevelsCount())
		return -1;

	return pLevelVisibleFacesCount[i];
}

int PatchFacesIndexer::GetLevelInvisibleFacesCount(int i) const
{
	if (NULL == pLevelInvisibleFacesCount)
		return -1;

	if (i < 0 || i >= patchResolution.GetLevelsCount())
		return -1;

	return pLevelInvisibleFacesCount[i];
}

int PatchFacesIndexer::GetLevelStartIndex(int i) const
{
	if (NULL == pLevelStartIndex)
		return -1;

	if (i < 0 || i >= patchResolution.GetLevelsCount())
		return -1;

	return pLevelStartIndex[i];
}

PatchFacesIndexer::PatchFacesIndexer(PatchResolution &patchResolution, int *pLVisFaceCount, int *pLInvFaceCount, int *pLStartIndex)
: patchResolution(patchResolution),
pLevelVisibleFacesCount(pLVisFaceCount),
pLevelInvisibleFacesCount(pLInvFaceCount),
pLevelStartIndex(pLStartIndex)
{
}

PatchFacesIndexer* PatchFacesIndexer::Create(PatchResolution *pPatchResolution)
{
	if (NULL == pPatchResolution)
		return NULL;

	int levelsCount = pPatchResolution->GetLevelsCount();

	// cache values of each level faces count
	int *pVis = new int[levelsCount];	// visible faces count
	if (NULL == pVis)
		return NULL;

	int *pInv = new int[levelsCount];	// invisible faces count
	if (NULL == pInv)
	{
		delete [] pVis;
		return NULL;
	}

	int *pStartIndex = new int[levelsCount];
	if (NULL == pStartIndex)
	{
		delete [] pVis;
		delete [] pInv;
		return NULL;
	}

	int n = 0;
	for (int i = 0; i < levelsCount; ++i)
	{
		pStartIndex[i] = n;
		pVis[i] = (int)pow(2.0f, 2 * i + 1);
		pInv[i] = (int)pow(2.0f, i + 2) - 4;
		n += pVis[i] + pInv[i] + 2;
	}

	return new PatchFacesIndexer(*pPatchResolution, pVis, pInv, pStartIndex);
}

PatchFacesIndexer::~PatchFacesIndexer()
{
	if (NULL != pLevelStartIndex)
		delete [] pLevelStartIndex;

	if (NULL != pLevelInvisibleFacesCount)
		delete [] pLevelInvisibleFacesCount;

	if (NULL != pLevelVisibleFacesCount)
		delete [] pLevelVisibleFacesCount;
}

int PatchFacesIndexer::LevelFacesCount(int i) const
{
	return GetLevelVisibleFacesCount(i) + GetLevelInvisibleFacesCount(i);
}

int PatchFacesIndexer::FacesOfAllLevelsCount() const
{
	int sum = 0;
	for (int i = 0; i < patchResolution.GetLevelsCount(); ++i)
		sum += LevelFacesCount(i);

	return sum;
}

int PatchFacesIndexer::LevelIndicesCount(int i) const
{
	return LevelFacesCount(i) + 2;
}

int PatchFacesIndexer::IndicesOfAllLevelsCount() const
{
	return FacesOfAllLevelsCount() + 2 * patchResolution.GetLevelsCount();
}

void PatchFacesIndexer::GenerateIndices(WORD *pBuffer, const PatchVerticesMapping &pvm) const
{
	int levelsCount = patchResolution.GetLevelsCount();
	int patchRes = patchResolution.GetValue();
	int m, ctr = 0;

	for (int i = 0; i < levelsCount; ++i)
	{
		m = (int)pow(2.0f, levelsCount - i - 1);
		for (int row = 0; row < patchRes; row += m)
		{
			for (int col = 0; col <= patchRes; col += m)
			{
				pBuffer[ctr++] = (WORD)pvm(row, col);
				pBuffer[ctr++] = (WORD)pvm(row + m, col);
			}

			if (row + m < patchRes)
			{
				pBuffer[ctr++] = (WORD)pvm(row + m, patchRes);
				pBuffer[ctr++] = (WORD)pvm(row + m, 0);
			}
		}
	}
}