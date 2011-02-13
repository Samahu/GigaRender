#include "dxstdafx.h"
#include "PatchResolution.h"
#include "PatchVerticesMapping.h"

const PatchResolution& PatchVerticesMapping::GetPatchResolution() const
{
	return patchResolution;
}

int PatchVerticesMapping::GetVerticesCountPerPatchRow() const
{
	return patchResolution.GetValue() + 1;
}

int PatchVerticesMapping::GetVerticesCountPerPatch() const
{
	return GetVerticesCountPerPatchRow() * GetVerticesCountPerPatchRow();
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
	directMapping = new int*[GetVerticesCountPerPatchRow()];
    for (int i = 0; i < GetVerticesCountPerPatchRow(); ++i)
	{
		directMapping[i] = new int[GetVerticesCountPerPatchRow()];
        for (int j = 0; j < GetVerticesCountPerPatchRow(); ++j)	// initialize with an invalid value.
            directMapping[i][j] = -1;
	}
	reverseMapping = new Point2I[GetVerticesCountPerPatch()];

    int w = patchResolution.GetValue();
    int ctr = 0;
    int level = 0;
    pLevelVerticesCount = new int[patchResolution.GetLevelsCount()];

    do
    {
        for (int i = 0; i < GetVerticesCountPerPatchRow(); i += w)
            for (int j = 0; j < GetVerticesCountPerPatchRow(); j += w)
                if (-1 == directMapping[i][j])
				{
					reverseMapping[ctr].i = i;
					reverseMapping[ctr].j = j;
                    directMapping[i][j] = ctr++;
				}

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

	if (NULL != directMapping)
	{
		for (int i = 0; i < GetVerticesCountPerPatchRow(); ++i)
			if (NULL != directMapping[i])
				delete [] directMapping[i];

		delete [] directMapping;
	}

	if (NULL != reverseMapping)
		delete [] reverseMapping;
}


int PatchVerticesMapping::operator()(int row, int col) const
{
    assert(row >= 0 && row <= patchResolution.GetValue() &&
        col >= 0 && col <= patchResolution.GetValue());

    return directMapping[row][col];
}

void PatchVerticesMapping::ReverseMappingOf(int index, int &i, int &j) const
{
	assert(index >= 0 && index < GetVerticesCountPerPatch());

	i = reverseMapping[index].i;
	j = reverseMapping[index].j;
}