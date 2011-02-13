#include "stdafx.h"
#include "PatchResolution.h"
#include "PatchVerticesMapping.h"
#include <assert.h>

const PatchResolution& PatchVerticesMapping::GetPatchResolution() const
{
	return patchResolution;
}

int PatchVerticesMapping::GetVerticesCountPerRow() const
{
	return patchResolution.GetValue() + 1;
}

int PatchVerticesMapping::GetVerticesCountPerPatch() const
{
	return GetVerticesCountPerRow() * GetVerticesCountPerRow();
}

int PatchVerticesMapping::GetAccumulatedLevelVerticesCount(int level) const
{
	assert(NULL != pLevelVerticesCount);
	assert(level >= 0 && level < patchResolution.GetLevelsCount());

	return pLevelVerticesCount[level];
}

int PatchVerticesMapping::GetNonAccumulatedLevelVerticesCount(int level) const
{
	assert(NULL != pLevelVerticesCount);
	assert(level >= 0 && level < patchResolution.GetLevelsCount());

	return (0 == level ? pLevelVerticesCount[0] : pLevelVerticesCount[level] - pLevelVerticesCount[level - 1]);
}

// Private Constructor
PatchVerticesMapping::PatchVerticesMapping(PatchResolution &patchResolution)
	: patchResolution(patchResolution)
{
    pData = new int*[patchResolution.GetValue() + 1];
    for (int i = 0; i <= patchResolution.GetValue(); ++i)
	{
		pData[i] = new int[patchResolution.GetValue() + 1];
        for (int j = 0; j <= patchResolution.GetValue(); ++j)	// initialize with an invalid value.
            pData[i][j] = -1;
	}

    int w = patchResolution.GetValue();
    int ctr = 0;
    int level = 0;
    pLevelVerticesCount = new int[patchResolution.GetLevelsCount()];

    do
    {
        for (int i = 0; i <= patchResolution.GetValue(); i += w)
            for (int j = 0; j <= patchResolution.GetValue(); j += w)
                if (-1 == pData[i][j])
                    pData[i][j] = ctr++;

        pLevelVerticesCount[level++] = ctr;
    } while ((w /= 2) != 0);
}

PatchVerticesMapping* PatchVerticesMapping::Create(PatchResolution* pPatchResolution)
{
    if (NULL == pPatchResolution)
        return NULL;

    return new PatchVerticesMapping(*pPatchResolution);
}

PatchVerticesMapping::~PatchVerticesMapping()
{
	if (NULL != pLevelVerticesCount)
		delete [] pLevelVerticesCount;

	if (NULL != pData)
	{
		for (int i = 0; i <= patchResolution.GetValue(); ++i)
			if (NULL != pData[i])
				delete [] pData[i];

		delete [] pData;
	}
}


int PatchVerticesMapping::operator()(int row, int col) const
{
    if (row < 0 || row > patchResolution.GetValue() ||
        col < 0 || col > patchResolution.GetValue())
        return -1;

    return pData[row][col];
}