#include "dxstdafx.h"
#include "patch.h"
#include "TerrainPager.h"
#include "PatchResolution.h"
#include "PatchVerticesMapping.h"
#include "PatchFacesIndexer.h"
#include "CriticalSection.h"

TerrainPager & Patch::GetTerrainPager()
{
	return terrainPager;
}

int Patch::GetNbLoadedVertices() const
{
	int copy;
	csNbLoadedVertices->Enter();
	copy = nbLoadedVertices;
	csNbLoadedVertices->Leave();
	return copy;
}

int Patch::GetNbDecompressedVertices() const
{
	int copy;
	csNbDecompressedVertices->Enter();
	copy = nbDecompressedVertices;
	csNbDecompressedVertices->Leave();
	return copy;
}

int Patch::GetNbCopiedVertices() const
{
	int copy;
	csNbCopiedVertices->Enter();
	copy = nbCopiedVertices;
	csNbCopiedVertices->Leave();
	return copy;
}

void Patch::SetNbLoadedVertices(int value)
{
	csNbLoadedVertices->Enter();
	nbLoadedVertices = value;
	csNbLoadedVertices->Leave();
}

void Patch::SetNbDecompressedVertices(int value)
{
	csNbDecompressedVertices->Enter();
	nbDecompressedVertices = value;
	csNbDecompressedVertices->Leave();
}

void Patch::SetNbCopiedVertices(int value)
{
	csNbCopiedVertices->Enter();
	nbCopiedVertices = value;
	updateCurrentLoadedLOD();
	csNbCopiedVertices->Leave();
}

int Patch::GetBaseVertexIndex() const
{
	return baseVertexIndex;
}

int Patch::GetLevelOfDetail() const
{
	return levelOfDetail;
}

int Patch::GetCurrentLoadedLOD() const
{
	csCurrentLoadedLOD->Enter();
	int copy = currentLoadedLOD;
	csCurrentLoadedLOD->Leave();
	return copy;
}

bool Patch::IsTextureLoaded() const
{
	csTextureLoaded->Enter();
	bool copy = textureLoaded;
	csTextureLoaded->Leave();
	return copy;
}

void Patch::SetTextureLoaded(bool value)
{
	csTextureLoaded->Enter();
	textureLoaded = value;
	csTextureLoaded->Leave();
}

bool Patch::IsLODLoaded(int level) const
{
	return GetCurrentLoadedLOD() >= level;
}

bool Patch::AreAllLODsLoaded() const
{
	return GetCurrentLoadedLOD() == terrainPager.GetPatchResolution().GetMaxLevel();
}

int Patch::GetMustLoadLevelOfDetailsCount() const
{
	return levelOfDetail - GetCurrentLoadedLOD();
}

unsigned int Patch::GetIndex() const
{
	return index;
}

unsigned int Patch::GetReferenceCount() const
{
	return referenceCount;
}

void Patch::IncrementReferenceCount()
{
	++referenceCount;
}

unsigned int Patch::DecrementReferenceCount()
{
	return --referenceCount;
}

std::list< Patch * >::iterator & Patch::GetIterator()
{
	return iterator;
}

void Patch::SetIterator(std::list< Patch * >::iterator &iterator)
{
	this->iterator = iterator;
}

void Patch::SetID(const PatchID &patchID)
{
	this->patchID = patchID;
}

const PatchID & Patch::GetID() const
{
	return patchID;
}

void Patch::SetPriority(int priority)
{
	this->priority = priority;
}

int Patch::GetPriority() const
{
	return priority;
}

void Patch::setCurrentLoadedLOD(int lod)
{
	csCurrentLoadedLOD->Enter();
	currentLoadedLOD = lod;
	csCurrentLoadedLOD->Leave();
}

void Patch::updateCurrentLoadedLOD()
{
	int levelsCount = terrainPager.GetPatchResolution().GetLevelsCount();
	const PatchVerticesMapping &vm = terrainPager.GetPatchVerticesMapping();

	for (int level = GetCurrentLoadedLOD() + 1; level < levelsCount; ++level)
		if (nbCopiedVertices < vm.GetAccumulatedLevelVerticesCount(level))
		{
			setCurrentLoadedLOD(level - 1);
			return;
		}

	setCurrentLoadedLOD(levelsCount - 1);	// max level
}

Patch::Patch(TerrainPager &terrainPager, unsigned int patchIndex, int baseVertexIndex, IDirect3DTexture9 *texture)
: terrainPager(terrainPager), index(patchIndex), baseVertexIndex(baseVertexIndex), textureVidMem(texture)
{
	currentLoadedLOD = -1;
	nbLoadedVertices = 0;
	nbDecompressedVertices = 0;
	nbCopiedVertices = 0;
	csCurrentLoadedLOD = CriticalSection::Create();
	csNbLoadedVertices = CriticalSection::Create();
	csNbDecompressedVertices = CriticalSection::Create();
	csNbCopiedVertices = CriticalSection::Create();
	csTextureLoaded = CriticalSection::Create();
	csPendingRequests = CriticalSection::Create();
	priority = 0;
	referenceCount = 0;
	textureLoaded = false;
	submittedRequestsCount = 0;

	patchHeights = NULL;
	vertexBufferSysMem = NULL;
	imageBuffer = NULL;
	imageSize = 0;
	textureSysMem = NULL;
}

Patch::~Patch()
{
	if (NULL != patchHeights)
		delete [] patchHeights;

	if (NULL != vertexBufferSysMem)
		delete [] vertexBufferSysMem;

	if (NULL != imageBuffer)
		delete [] imageBuffer;

	if (NULL != textureSysMem)
		textureSysMem->Release();

	if (NULL != textureVidMem)
		textureVidMem->Release();

	if (NULL != csPendingRequests)
		delete csPendingRequests;

	if (NULL != csNbLoadedVertices)
		delete csNbLoadedVertices;

	if (NULL != csNbDecompressedVertices)
		delete csNbDecompressedVertices;

	if (NULL != csNbCopiedVertices)
		delete csNbCopiedVertices;

	if (NULL != csCurrentLoadedLOD)
		delete csCurrentLoadedLOD;

	if (NULL != csTextureLoaded)
		delete csTextureLoaded;
}

void Patch::Reset()
{
	// reset variables
	SetNbLoadedVertices(0);
	SetNbDecompressedVertices(0);
	SetNbCopiedVertices(0);
	setCurrentLoadedLOD(-1);
	SetTextureLoaded(false);
	imageSize = 0;
}

void Patch::Render(IDirect3DDevice9 &device)
{
	//TODO: need to synchronize this variable.
	int lod = GetCurrentLoadedLOD();
	if (lod < 0)
		return;

	bool isTexLoaded = IsTextureLoaded();

	int nbVertices = terrainPager.GetPatchVerticesMapping().GetAccumulatedLevelVerticesCount(lod);
	int nbFaces = terrainPager.GetPatchFacesIndexer().LevelFacesCount(lod);
	int startIndex = terrainPager.GetPatchFacesIndexer().GetLevelStartIndex(lod);

	HRESULT hr;
	V(device.SetStreamSource(0, terrainPager.GetVertexBuffer(), baseVertexIndex * sizeof(TerrainVertex), sizeof(TerrainVertex)));
	V(device.SetTexture(0, (isTexLoaded ? textureVidMem : NULL)));
	V(device.DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, nbVertices, startIndex, nbFaces));
}

void Patch::PendRequest(RequestID* requestID)
{
	csPendingRequests->Enter();
	pendingRequests.push_back(requestID);
	csPendingRequests->Leave();
}

std::list<RequestID *> & Patch::LockPendingRequests()
{
	csPendingRequests->Enter();
	return pendingRequests;
}

void Patch::UnlockPendingRequests()
{
	csPendingRequests->Leave();
}

void Patch::IncrementSubmittedRequestsCount()
{
	++submittedRequestsCount;
}

unsigned int Patch::DecrementSubmittedRequestCount()
{
	return --submittedRequestsCount;
}