#pragma once

class PatchResolution;
class PatchVerticesMapping;
class PatchFacesIndexer;
class PatchID;
class Patch;
class WorkingSetWindow;
class StreamReader;
class WorkQueue;
class JobManager;

struct TerrainVertex
{
	D3DXVECTOR3 p;
	D3DCOLOR	c;

	static const int FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

#define PATCH_TEXTURE_WIDTH 256
#define PATCH_TEXTURE_HEIGHT 256

class TerrainPager
{
private:
	typedef std::list< Patch * > PATCH_LIST;
	typedef PATCH_LIST::iterator PATCH_LIST_ITERATOR;

	unsigned int nbAllocatedPatches;

	bool utilizeTextures;
	IDirect3DVertexDeclaration9 *vertexDeclTextured;
	IDirect3DVertexDeclaration9 *vertexDeclTextureless;
	IDirect3DVertexBuffer9 *vbTexUV;


	Patch** pPatches;							// patch pool.
	std::queue< unsigned int > readyPatchQueue;
	PATCH_LIST workingSet;						// list of active patches

	PatchResolution *pPatchResolution;
	PatchVerticesMapping *pPatchVerticesMapping;
	PatchFacesIndexer *pPatchFacesIndexer;

	int resolution;
	int hPatchesCount;				// patches count per file row.
	int vPatchesCount;				// total rows within the file.
	float stepX;					// distance between adjacent vertices
	float stepZ;					// distance between adjacent vertices
	float heightScale;				// the applied height scale.
	int maxImageSize;				// 

	StreamReader *geometryStream;
	StreamReader *texturesStream;

	IDirect3DVertexBuffer9 *pVB;	// shared vertex buffer for all patches.
	IDirect3DIndexBuffer9 *pIB;		// index buffer containing faces data for the patch of all LODs.
	IDirect3DDevice9 *pDevice;		// direct3d device handle.

	JobManager *jobManager;

public:
	void SetDevice(IDirect3DDevice9* pDevice);
	IDirect3DDevice9 & GetDevice();
	const PatchResolution & GetPatchResolution() const;
	const PatchVerticesMapping & GetPatchVerticesMapping() const;
	const PatchFacesIndexer & GetPatchFacesIndexer() const;
	float GetStepX() const { return stepX; }
	float GetStepZ() const { return stepZ; }
	float GetHeightScale() const { return heightScale; }
	int GetMaxImageSize() const { return maxImageSize; }
	float GetPatchWidth() const { return resolution * stepX; }
	float GetPatchHeight() const { return resolution * stepZ; }
	unsigned int GetNbAllocatedPatches() const;
	IDirect3DVertexBuffer9 * GetVertexBuffer();
	StreamReader & GetGeometryStream();
	StreamReader & GetTexturesStream();

	int GetHPatchesCount() const { return hPatchesCount; }	// count of patches within a single row of the file.
	int GetVPatchesCount() const { return vPatchesCount; }	// count of rows within the file.

private:
	void createVertexDeclarations();
	bool createIndices();
	bool createPatches(int nbVerticesPerPatch);
	void deletePatches();
	Patch * findPatch(const PatchID &patchID);	// searches for the patch within the working set
											// if it can't find it returns NULL pointer.
	// Constructor
	TerrainPager();

public:
	// @param filename: name of the major file.
	static TerrainPager * Create(char const filename[], unsigned int nbAllocatedPatches);

	// Destructor
	~TerrainPager();

	bool Init();
	void Destroy();

	TerrainVertex * LockVertexBuffer(int offsetToLock, int sizeToLock, DWORD flags);
	void UnlockVertexBuffer();

	Patch * GetPatch(const PatchID &patchID);
	void FreePatch(Patch *pPatch);
	void FreePatch(const PatchID &patchID);

	void Process(float timeDelta);
	void Render();
};