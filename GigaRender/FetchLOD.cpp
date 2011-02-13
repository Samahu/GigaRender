#include "dxstdafx.h"
#include "FetchLOD.h"
#include "TerrainPager.h"
#include "Patch.h"
#include "PatchVerticesMapping.h"
#include "StreamReader.h"

FetchLOD::FetchLOD(Patch &target, unsigned int requestedLOD)
: FetchPatchItem(target), requestedLOD(requestedLOD)
{
}

void FetchLOD::LoadToSystemMemory()
{
	TerrainPager &pager = target.GetTerrainPager();
	const PatchVerticesMapping &mapping = pager.GetPatchVerticesMapping();
	int vrtsToFillCount = mapping.GetNonAccumulatedLevelVerticesCount(requestedLOD);

	// truncate the passed number to the a valid one.
	int nbVrtsPerPatch = mapping.GetVerticesCountPerPatch();
	int nbLoadedVertices = target.GetNbLoadedVertices();
	int remainingVrtsCount = nbVrtsPerPatch - nbLoadedVertices;
	if (0 == remainingVrtsCount)
		return;

	if (remainingVrtsCount < vrtsToFillCount)
		vrtsToFillCount = remainingVrtsCount;


	unsigned int offsetToLock = nbLoadedVertices * sizeof(float);
	unsigned int sizeToLock = vrtsToFillCount * sizeof(float);

	assert(offsetToLock + sizeToLock <= nbVrtsPerPatch * sizeof(float));

	StreamReader &geometryStream = pager.GetGeometryStream();

	// seek to the location of the patch within the file and pass already read vertices.
	const PatchID &id = target.GetID();
	int patchOffset = (pager.GetHPatchesCount() * id.GetI() + id.GetJ()) * nbVrtsPerPatch * sizeof(float);
	geometryStream.Seek(patchOffset + nbLoadedVertices * sizeof(float), StreamReader::SeekBegin);

	// Reading into a memory buffer.
	float* dst = (float*)((char*)target.patchHeights + offsetToLock);
	geometryStream.Read(dst, sizeToLock, sizeof(float), vrtsToFillCount);

	nbLoadedVertices += vrtsToFillCount;
	target.SetNbLoadedVertices(nbLoadedVertices);
}

void FetchLOD::Decompress()
{
	TerrainPager &pager = target.GetTerrainPager();
	static const float stepX = pager.GetStepX();
	static const float stepZ = pager.GetStepZ();
	static const float heightScale = pager.GetHeightScale();
	int nbLoadedVrts = target.GetNbLoadedVertices();
	int nbDecompressedVrts = target.GetNbCopiedVertices();
	int vrtsToDecompress = nbLoadedVrts - nbDecompressedVrts;
	if (vrtsToDecompress <= 0)
		return;

	const PatchVerticesMapping &mapping = pager.GetPatchVerticesMapping();

	float *nonDecompressedStart = target.patchHeights + nbDecompressedVrts;
	unsigned int offsetSysMem = nbDecompressedVrts * sizeof(TerrainVertex);
	TerrainVertex* buffer = (TerrainVertex*)(target.vertexBufferSysMem + offsetSysMem);
	for (int s = 0; s < vrtsToDecompress; ++s)
	{
		int index = nbDecompressedVrts + s;	// index of the relative to the start of the buffer.
		int i, j;
		mapping.ReverseMappingOf(index, i, j);
		buffer[s].p.y = nonDecompressedStart[s] * heightScale;
		buffer[s].p.z = (target.GetID().GetI() * (mapping.GetVerticesCountPerPatchRow() - 1)+ i) * stepZ;
		buffer[s].p.x = (target.GetID().GetJ() * (mapping.GetVerticesCountPerPatchRow() - 1)+ j) * stepX;
		buffer[s].c = D3DCOLOR_ARGB(255, 255, 255, 255);
	}

	nbDecompressedVrts += vrtsToDecompress;
	target.SetNbDecompressedVertices(nbDecompressedVrts);
}

void FetchLOD::CopyToVideoMemory()
{
	TerrainPager &pager = target.GetTerrainPager();
	int nbDecompressedVrts = target.GetNbDecompressedVertices();
	int nbCopiedVrts = target.GetNbCopiedVertices();
	int vrtsToCopy = nbDecompressedVrts - nbCopiedVrts;
	if (vrtsToCopy <= 0)
		return;

	unsigned int offsetSysMem = nbCopiedVrts * sizeof(TerrainVertex);
	char* src = target.vertexBufferSysMem + offsetSysMem;
	unsigned int offsetToLock = (target.GetBaseVertexIndex() + nbCopiedVrts) * sizeof(TerrainVertex);
	unsigned int sizeToLock = vrtsToCopy * sizeof(TerrainVertex);

	TerrainVertex* dst = pager.LockVertexBuffer(offsetToLock, sizeToLock, D3DLOCK_NOOVERWRITE);
	memcpy(dst, src, sizeToLock);
	pager.UnlockVertexBuffer();

	nbCopiedVrts += vrtsToCopy;
	target.SetNbCopiedVertices(nbCopiedVrts);
}