#include "dxstdafx.h"
#include "FetchTexture.h"
#include "TerrainPager.h"
#include "Patch.h"
#include "StreamReader.h"

FetchTexture::FetchTexture(Patch &target)
: FetchPatchItem(target)
{
}

void FetchTexture::LoadToSystemMemory()
{
	TerrainPager &pager = target.GetTerrainPager();
	int maxImageSize = pager.GetMaxImageSize();
	StreamReader &texturesStream = pager.GetTexturesStream();

	const PatchID &patchID = target.GetID();

	// read the offset and size of this patch within the terrainTextruesStream
	int patchIndex = patchID.GetI() * pager.GetHPatchesCount() + patchID.GetJ();
	texturesStream.Seek(patchIndex * 2 * sizeof(int), StreamReader::SeekBegin);

	int imageOffset, imageSize;
	texturesStream.Read(&imageOffset, sizeof(int), sizeof(int), 1);
	texturesStream.Read(&imageSize, sizeof(int), sizeof(int), 1);
	assert(imageSize <= maxImageSize);

	// load the image into the buffer.
	texturesStream.Seek(imageOffset, StreamReader::SeekBegin);
	texturesStream.Read(target.imageBuffer, maxImageSize, sizeof(byte), imageSize);
	target.imageSize = imageSize;
}

void FetchTexture::Decompress()
{
	IDirect3DSurface9 *surface;
	if SUCCEEDED(target.textureSysMem->GetSurfaceLevel(0, &surface))
	{
		HRESULT hr;
		V(D3DXLoadSurfaceFromFileInMemory(surface, NULL, NULL, target.imageBuffer, target.imageSize, NULL, D3DX_DEFAULT, 0, NULL));
		surface->Release();
	}
}

void FetchTexture::CopyToVideoMemory()
{
	HRESULT hr;
	TerrainPager &pager = target.GetTerrainPager();
	V(pager.GetDevice().UpdateTexture(target.textureSysMem, target.textureVidMem));
	target.SetTextureLoaded(true);
}