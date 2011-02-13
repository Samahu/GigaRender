#include "dxstdafx.h"
#include "TerrainPager.h"
#include "Patch.h"
#include "PatchResolution.h"
#include "PatchVerticesMapping.h"
#include "PatchFacesIndexer.h"
#include "WorkingSetWindow.h"
#include "StreamReader.h"
#include "JobManager.h"

void TerrainPager::SetDevice(IDirect3DDevice9 *pDevice)
{
	this->pDevice = pDevice;
}

const PatchResolution & TerrainPager::GetPatchResolution() const
{
	return *pPatchResolution;
}

const PatchVerticesMapping & TerrainPager::GetPatchVerticesMapping() const
{
	return *pPatchVerticesMapping;
}

const PatchFacesIndexer & TerrainPager::GetPatchFacesIndexer() const
{
	return *pPatchFacesIndexer;
}

IDirect3DDevice9 & TerrainPager::GetDevice()
{
	return *pDevice;
}

unsigned int TerrainPager::GetNbAllocatedPatches() const
{
	 return nbAllocatedPatches;
}

IDirect3DVertexBuffer9 * TerrainPager::GetVertexBuffer()
{
	return pVB;
}

void initVertexElement(D3DVERTEXELEMENT9& element,
					   WORD stream,
					   WORD offset,
					   BYTE type,
					   BYTE method,
					   BYTE usage,
					   BYTE usageIndex)
{
	element.Stream = stream;
	element.Offset = offset;
	element.Type = type;
	element.Method = method;
	element.Usage = usage;
	element.UsageIndex = usageIndex;
}

void TerrainPager::createVertexDeclarations()
{
	HRESULT hr;

	D3DVERTEXELEMENT9 elements[4];
	D3DVERTEXELEMENT9 declEnd = D3DDECL_END();

	initVertexElement(elements[0], 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
	initVertexElement(elements[1], 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
	elements[2] = declEnd;
	V(pDevice->CreateVertexDeclaration(elements, &vertexDeclTextureless));

	initVertexElement(elements[2], 1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
	elements[3] = declEnd;
	V(pDevice->CreateVertexDeclaration(elements, &vertexDeclTextured));
}

bool TerrainPager::Init()
{
	assert(NULL != pDevice && "Direct 3D Device not set");
	geometryStream = StreamReader::Create("map");
	assert(NULL != geometryStream);	// No data to read just quit.
	if (utilizeTextures)
	{
		texturesStream = StreamReader::Create("textures");
		assert(NULL != texturesStream);	// No data to read just quit.
	}
	
	

	pPatchResolution = PatchResolution::Create(resolution);
	if (NULL == pPatchResolution)
		return false;

	pPatchVerticesMapping = PatchVerticesMapping::Create(pPatchResolution);
	if (NULL == pPatchVerticesMapping)
		return false;

	pPatchFacesIndexer = PatchFacesIndexer::Create(pPatchResolution);
	if (NULL == pPatchFacesIndexer)
		return false;

	createVertexDeclarations();

	// Now create device objects
	// determine vertex buffer size.
	int vrtsPerPatchRow = pPatchResolution->GetValue() + 1;
	int vrtsPerPatch = vrtsPerPatchRow * vrtsPerPatchRow;
	int vrtsInAllPatches = vrtsPerPatch * nbAllocatedPatches;

	HRESULT hr;


	// create texture uv vertex buffer
	V(pDevice->CreateVertexBuffer(
		vrtsPerPatch * 2 * sizeof(float),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_DEFAULT,
		&vbTexUV,
		NULL));

	// fill the uv vertex buffer.
	struct UV
	{
		float u, v;
	};

	UV *texUV;
	float step = 1.0f / pPatchResolution->GetValue();
	int index;
	V(vbTexUV->Lock(0, 0, (void**)&texUV, 0));
	for(int i = 0; i <= pPatchResolution->GetValue(); ++i)
		for (int j = 0; j <= pPatchResolution->GetValue(); ++j)
		{
			index = (*pPatchVerticesMapping)(i, j);
			texUV[index].u = j * step;
			texUV[index].v = i * step;
		}
	V(vbTexUV->Unlock());

	// create geomtery vertex buffer.
	V(pDevice->CreateVertexBuffer(
		vrtsInAllPatches * sizeof(TerrainVertex),
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_DEFAULT,
		&pVB,
		NULL));

	if (false == createPatches(vrtsPerPatch))
		return false;

	if (false == createIndices())
		return false;

	jobManager = new JobManager(pPatchResolution->GetLevelsCount(), utilizeTextures);

	return true;
}

bool TerrainPager::createIndices()
{
	HRESULT hr;

	// calculate the total buffer needed to fit all the indices needed to render those patches
	// currently only full patch indices are calculated
	int buffersize = pPatchFacesIndexer->IndicesOfAllLevelsCount() * sizeof(WORD);

	V(pDevice->CreateIndexBuffer(buffersize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB, NULL));

	WORD* pInds;
	pIB->Lock(0, 0, (void**)&pInds, 0);
	pPatchFacesIndexer->GenerateIndices(pInds, *pPatchVerticesMapping);
	pIB->Unlock();

	return true;
}

bool TerrainPager::createPatches(int nbVerticesPerPatch)
{
	assert(NULL != pDevice);

	IDirect3DTexture9 *texture = NULL;
	const D3DSURFACE_DESC * pBBDesc = DXUTGetBackBufferSurfaceDesc();

	// create patches and assign them to their files
	pPatches = new Patch*[nbAllocatedPatches];
	int baseVertexIndex = 0;

	HRESULT hr;

	for (unsigned int i = 0; i < nbAllocatedPatches; ++i)
	{
		if (utilizeTextures)
			V(pDevice->CreateTexture(PATCH_TEXTURE_WIDTH, PATCH_TEXTURE_HEIGHT, 1, D3DUSAGE_DYNAMIC, pBBDesc->Format, D3DPOOL_DEFAULT, &texture, NULL));

		pPatches[i] = new Patch(*this, i, baseVertexIndex, texture);
		if (utilizeTextures)
		{
			V(pDevice->CreateTexture(PATCH_TEXTURE_WIDTH, PATCH_TEXTURE_HEIGHT, 1, D3DUSAGE_DYNAMIC, pBBDesc->Format, D3DPOOL_SYSTEMMEM, &texture, NULL));
			pPatches[i]->textureSysMem = texture;
		}

		pPatches[i]->imageBuffer = new char[maxImageSize];
		pPatches[i]->vertexBufferSysMem = new char[nbVerticesPerPatch * sizeof(TerrainVertex)];
		pPatches[i]->patchHeights = new float[nbVerticesPerPatch];

		baseVertexIndex += nbVerticesPerPatch;
	}

	return true;
}

void TerrainPager::deletePatches()
{
	if (NULL != pPatches)
	{
		for (unsigned int i = 0; i < nbAllocatedPatches; ++i)
			if (NULL != pPatches[i])
				delete pPatches[i];

		delete [] pPatches;
	}
}

Patch * TerrainPager::findPatch(const PatchID &patchID)
{
	// TODO: find a better method than sequential search.
	Patch *pPatch;
	PATCH_LIST_ITERATOR t, e = workingSet.end();
	for (t = workingSet.begin(); t != e; ++t)
	{
		pPatch = *t;
		if (pPatch->GetID() == patchID)
			return pPatch;
	}

	return NULL;
}

TerrainPager::TerrainPager()
{
	pPatches = NULL;
	pVB = NULL;
	pIB = NULL;
	pDevice = NULL;
	pPatchResolution = NULL;
	pPatchVerticesMapping = NULL;
	pPatchFacesIndexer = NULL;
	geometryStream = NULL;
	texturesStream = NULL;
	utilizeTextures = true;
	jobManager = NULL;
}

TerrainPager * TerrainPager::Create(const char filename[], unsigned int nbMaxPages)
{
	if (nbMaxPages < 1)
		return NULL;

	// read patch resolution
	FILE* pfile = NULL;
	fopen_s(&pfile, filename, "rb");
	if (NULL == pfile)
		return NULL;

	TerrainPager *pTP = new TerrainPager();

	pTP->nbAllocatedPatches = nbMaxPages;
	for (unsigned int i = 0; i < nbMaxPages; ++i)
		pTP->readyPatchQueue.push(i);

	fread(&pTP->resolution, sizeof(int), 1, pfile);
	fread(&pTP->hPatchesCount, sizeof(int), 1, pfile);
	fread(&pTP->vPatchesCount, sizeof(int), 1, pfile);
	fread(&pTP->stepX, sizeof(float), 1, pfile);
	fread(&pTP->stepZ, sizeof(float), 1, pfile);
	fread(&pTP->heightScale, sizeof(float), 1, pfile);
	fread(&pTP->maxImageSize, sizeof(int), 1, pfile);

	fclose(pfile);

	return pTP;
}

TerrainPager::~TerrainPager()
{
	if (NULL != pPatchFacesIndexer)
		delete pPatchFacesIndexer;

	if (NULL != pPatchVerticesMapping)
		delete pPatchVerticesMapping;

	if (NULL != pPatchResolution)
		delete pPatchResolution;
}

void TerrainPager::Destroy()
{
	// we may need to cancel all pending requests before going shutdown.
	if(NULL != jobManager)
	{
		delete jobManager;
		jobManager = NULL;
	}

	deletePatches();


	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(vbTexUV);
	SAFE_RELEASE(vertexDeclTextured);
	SAFE_RELEASE(vertexDeclTextureless);

	// Close the stream
	if (NULL != geometryStream)
	{
		delete geometryStream;
		geometryStream = NULL;
	}

	if (NULL != texturesStream)
	{
		delete texturesStream;
		texturesStream = NULL;
	}
}

TerrainVertex * TerrainPager::LockVertexBuffer(int offsetToLock, int sizeToLock, DWORD flags)
{
	HRESULT hr;
	TerrainVertex* pVrts;
	V(pVB->Lock(offsetToLock, sizeToLock, (void**)&pVrts, flags));
	return pVrts;
}

void TerrainPager::UnlockVertexBuffer()
{
	HRESULT hr;
	V(pVB->Unlock());
}

Patch * TerrainPager::GetPatch(const PatchID &patchID)
{
	if (patchID.GetI() < 0 || patchID.GetJ() < 0)	// reject call
		return NULL;

	// Check the working set to see if it is loaded
	Patch *pPatch = findPatch(patchID);
	if (NULL != pPatch)
	{
		pPatch->IncrementReferenceCount();
		return pPatch;
	}

	// not within the working set.
	// go get it.

	// get a free slot.
	assert(!readyPatchQueue.empty());	// uptil now we just assume that we have a patch pool that is sufficient to our needs.
	unsigned int index = readyPatchQueue.front();
	readyPatchQueue.pop();				// mark this patch as occupied.

	// load the patch into this slot
	pPatch = pPatches[index];
	pPatch->Reset();					// reset internal values of nbLoadedVertices, currentLoadedLevel.
	pPatch->SetID(patchID);

	jobManager->RequestFillPatch(*pPatch, utilizeTextures);

	// push it onto the list
	workingSet.push_back(pPatch);
	pPatch->IncrementReferenceCount();			// this one is used.
	pPatch->SetIterator(--workingSet.end());	// the iterator of the latest added item.
	
	return pPatch;
}

void TerrainPager::FreePatch(Patch *pPatch)
{
	assert(pPatch->GetReferenceCount() > 0);	// the patch must be used

	if (pPatch->DecrementReferenceCount() > 0)
		return;	// still used by some other window.

	// reference count reached zero.
	PATCH_LIST_ITERATOR &it = pPatch->GetIterator();
	workingSet.erase(it);
	readyPatchQueue.push(pPatch->GetIndex());	// recycle.
}

void TerrainPager::FreePatch(const PatchID &patchID)
{
	if (patchID.GetI() < 0 || patchID.GetJ() < 0)	// reject call
		return;

	// Check the working set to see if it is loaded
	Patch *pPatch = findPatch(patchID);
	assert(NULL != pPatch);
	FreePatch(pPatch);
}

void TerrainPager::Render()
{
	if (NULL == pDevice)
		return;

	if (utilizeTextures)
	{
		pDevice->SetVertexDeclaration(vertexDeclTextured);
		pDevice->SetStreamSource(1, vbTexUV, 0, 2 * sizeof(float));
	}
	else
		pDevice->SetVertexDeclaration(vertexDeclTextureless);

	pDevice->SetIndices(pIB);

	// currently rendering all of the working set without culling
	for (PATCH_LIST::iterator i = workingSet.begin(); i != workingSet.end(); ++i)
		(*i)->Render(*pDevice);
}

StreamReader & TerrainPager::GetGeometryStream()
{
	return *geometryStream;
}

StreamReader & TerrainPager::GetTexturesStream()
{
	return *texturesStream;
}

void TerrainPager::Process(float timeDelta)
{
	jobManager->CopyItems();
}