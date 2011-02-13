#pragma once

#include "PatchID.h"

class TerrainPager;
class CriticalSection;
class RequestID;
class Visitor;

class Patch
{
private:
	TerrainPager &terrainPager;
	int nbLoadedVertices;		// represents the count of patch heights currently loaded within the system memory.
	int nbDecompressedVertices; // represents the count of vertices currently within the system memory & ready to be copied.
	int nbCopiedVertices;		// represents the count of vertices currently loaded within the vertex buffer.
	int currentLoadedLOD;
	int baseVertexIndex;	// place to start storing vertices at.
	int levelOfDetail;		// the current level of detail that this patch should achive.

	// data used by the terrain pager.
	unsigned int index;
	unsigned int referenceCount;
	unsigned int submittedRequestsCount;
	std::list< Patch * >::iterator iterator;	// the iterator holding this patch within the working set.
	PatchID patchID;
	int priority;

	std::list<RequestID *> pendingRequests;	// request scheduled to fill this patch.

	bool textureLoaded;

	CriticalSection *csNbLoadedVertices;
	CriticalSection *csNbDecompressedVertices;
	CriticalSection *csNbCopiedVertices;
	CriticalSection *csCurrentLoadedLOD;
	CriticalSection *csTextureLoaded;
	CriticalSection *csPendingRequests;

public:
	IDirect3DTexture9 *textureVidMem;
	bool isDirty;

	float* patchHeights;
	char* vertexBufferSysMem;
	char* imageBuffer;
	int imageSize;
	IDirect3DTexture9* textureSysMem;

public:
	TerrainPager & GetTerrainPager();
	int GetNbLoadedVertices() const;
	void SetNbLoadedVertices(int value);
	int GetNbDecompressedVertices() const;
	void SetNbDecompressedVertices(int value);
	int GetNbCopiedVertices() const;
	void SetNbCopiedVertices(int value);
	int GetBaseVertexIndex() const;
	int GetLevelOfDetail() const;
	int GetCurrentLoadedLOD() const;
	bool IsTextureLoaded() const;
	void SetTextureLoaded(bool value);
	bool IsLODLoaded(int level) const;
	bool AreAllLODsLoaded() const;
	int GetMustLoadLevelOfDetailsCount() const;

	unsigned int GetIndex() const;
	unsigned int GetReferenceCount() const;
	void IncrementReferenceCount();
	unsigned int DecrementReferenceCount();		// returns the reference count after being decremented.
	void IncrementSubmittedRequestsCount();
	unsigned int DecrementSubmittedRequestCount();
	std::list< Patch * >::iterator & GetIterator();
	void SetIterator(std::list< Patch * >::iterator &iterator);
	void SetID(const PatchID &patchID);
	const PatchID & GetID() const;
	void SetPriority(int priority);
	int GetPriority() const;

private:
	void setCurrentLoadedLOD(int lod);
	void updateCurrentLoadedLOD();
	Patch & operator= (Patch &) {}	// removes warning: [assignment operator could not be generated].

public:
	// Constructor
	Patch(TerrainPager &terrainPager, unsigned int patchIndex, int baseVertexIndex, IDirect3DTexture9 *textrue);
	// Desturctor
	~Patch();

	void Reset();

	void Render(IDirect3DDevice9 &device);

	// adds the passed RequestID to the list of pending requests related to this patch.
	void PendRequest(RequestID* requestID);
	std::list<RequestID *> & LockPendingRequests();
	void UnlockPendingRequests();
};