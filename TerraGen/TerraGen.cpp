// Tutorial1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <list>

#include <atlimage.h>
#include <string>

#include <noise.h>
#include "noiseutils.h"

#include "PatchResolution.h"
#include "PatchVerticesMapping.h"

using namespace std;
using namespace noise::module;
using namespace noise::utils;

struct TerrainVertex
{
	float x, y, z;
	unsigned int c;
};

#define D3DCOLOR_ARGB(a,r,g,b) ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define clip(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

// allocate a imgBuffer of 1M size.
#define MAX_TEXTURE_FILE_SIZE 1024 * 1024
char imgBuffer[MAX_TEXTURE_FILE_SIZE];

/*
void	GenrateNormalTerrain()
{
	//=========================================
	//The Generator Module(s):
	//=========================================
	Perlin myModule;
	myModule.SetFrequency (0.5);
	myModule.SetPersistence (0.6);
	//=========================================
	

	//=========================================
	//The NoiseMap(s):
	//=========================================
	NoiseMap heightMap;
	//=========================================
	

	//=========================================
	//NoiseMapBuilder(s):
	//=========================================
	NoiseMapBuilderPlane heightMapBuilder;
	//=========================================
	

	//=========================================
	//The Algorithem:
	//=========================================
	//1- Build the HeightMap.
	
	//myModule.SetFrequency (3.0);

	heightMapBuilder.SetSourceModule (myModule);	//connect the Generator Moduler to the NoiseMapBuilder.
	heightMapBuilder.SetDestNoiseMap (heightMap);	//connect the NoiseMap ( the output )to the NoiseMapBuilder.
	heightMapBuilder.SetDestSize (1024, 1024);		//Set the size of the desiered image( output ).
	heightMapBuilder.SetBounds ( 6.0, 16.0, 1.0, 10.0 );// Set The oundaries.
	heightMapBuilder.Build ();						//Build the HeightMap and attach to the NoiseMap.
	//2- Write The HeightMap to an Image Variable.
	RendererImage renderer;
	Image image;
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	//Write the Image HeightMap to file.
	WriterBMP writer;
	writer.SetSourceImage (image);
	writer.SetDestFilename ("HeightMap.bmp");
	writer.WriteDestFile ();
	
	//=========================================
	//End of the Algo:
	//=========================================

	//=========================================
	//Render the TERRAIN.bmp Texture
	//=========================================
	renderer.ClearGradient ();
	renderer.AddGradientPoint (-1.00, Color ( 32, 160,   0, 255)); // grass
	renderer.AddGradientPoint (-0.25, Color (224, 224,   0, 255)); // dirt
	renderer.AddGradientPoint ( 0.25, Color (128, 128, 128, 255)); // rock
	renderer.AddGradientPoint ( 1.00, Color (255, 255, 255, 255)); // snow
	renderer.EnableLight ();
	renderer.SetLightContrast (2.0);
	renderer.SetLightBrightness (2.0);
	renderer.Render ();
	
	writer.SetSourceImage (image);
	writer.SetDestFilename ("TERRAIN.bmp");
	writer.WriteDestFile ();
}

void	GenrateValiesTerrain()
{
	RidgedMulti	mountainTerrain;		//Mountain Modifier.
	ScaleBias	mountainScaler;			//
	Billow		baseFlatTerrain;			//The Base Flat Terrain.
	Perlin		terrainType;				//The Flat Terrain.
	Select		finalTerrain;				//The Final Output of the Terain.
	ScaleBias	flatTerrain;			//Modifier for scaling.

	Perlin		LastTerrainType;				//The Flat Terrain.
	Select		LastfinalTerrain;				//The Final Output of the Terain.
	ScaleBias	LastflatTerrain;			//Modifier for scaling.	



	RidgedMulti	ValiesTerrain;
	module::ScaleBias	ValiesScaler;
	ValiesScaler.SetSourceModule (0,ValiesTerrain );
	ValiesScaler.SetScale ( 1.00 );
	ValiesScaler.SetBias( -1.5 );

	mountainScaler.SetSourceModule( 0, mountainTerrain );
	mountainScaler.SetScale ( 1 );
	mountainScaler.SetBias(0);

	mountainTerrain.SetSeed(1);		//Mountain Modifier.
	baseFlatTerrain.SetSeed(4);			//The Base Flat Terrain.
	terrainType.SetSeed(3);				//The Flat Terrain.
	ValiesTerrain.SetSeed(2);

	baseFlatTerrain.SetFrequency (2.0);

	flatTerrain.SetSourceModule ( 0, baseFlatTerrain );
	flatTerrain.SetScale ( 0.100 );
	flatTerrain.SetBias ( 0.0 );


	terrainType.SetFrequency (0.6);
	terrainType.SetPersistence (0.6);


	finalTerrain.SetSourceModule ( 0 , flatTerrain );
	finalTerrain.SetSourceModule ( 1 , mountainScaler );
	finalTerrain.SetControlModule( terrainType );
	finalTerrain.SetBounds ( -0.1, 1000.0 );
	finalTerrain.SetEdgeFalloff ( 0.225 );


	LastTerrainType.SetSeed( 30 );
	LastTerrainType.SetFrequency( 0.5 );
	LastTerrainType.SetPersistence( 0.5 );
	LastfinalTerrain.SetSourceModule( 0, finalTerrain );
	LastfinalTerrain.SetSourceModule( 1, ValiesScaler );
	LastfinalTerrain.SetControlModule( LastTerrainType );
	LastfinalTerrain.SetBounds ( 0.2, 1000.0);
	LastfinalTerrain.SetEdgeFalloff (0.425);

	NoiseMap heightMap;
	NoiseMapBuilderPlane heightMapBuilder;

	heightMapBuilder.SetSourceModule ( LastfinalTerrain );

	heightMapBuilder.SetDestNoiseMap ( heightMap );
	heightMapBuilder.SetDestSize (256, 256);
	heightMapBuilder.SetBounds ( 6.0, 11.0, 3.0, 8.0 );
	heightMapBuilder.Build ();

	RendererImage renderer;
	Image image;
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	WriterBMP writer1;
	writer1.SetSourceImage (image);
	writer1.SetDestFilename ("HeightMap.bmp");
	writer1.WriteDestFile ();

	renderer.ClearGradient ();
	
	renderer.AddGradientPoint (-1.0000, Color (  0,   0, 255, 255)); // deeps
	renderer.AddGradientPoint (-0.5500, Color (  0,   55, 200, 255)); // shallows
	renderer.AddGradientPoint (-0.4000, Color (  0,  70, 185, 255)); // shore
	renderer.AddGradientPoint (-0.2000, Color (140, 140,  0 , 255)); // sand
	renderer.AddGradientPoint ( 0.0000, Color ( 55, 200,   0, 255)); // grass
	renderer.AddGradientPoint ( 0.7000, Color (100, 100,   0, 255)); // dirt
	renderer.AddGradientPoint ( 1.0000, Color (128, 128,   0, 255)); // rock
	
	renderer.EnableLight ();
	renderer.SetLightElev(50);
	renderer.SetLightContrast (3.0);
	renderer.SetLightBrightness (1.0);
	renderer.Render ();

	WriterBMP writer;
	writer.SetSourceImage (image);
	writer.SetDestFilename ("TERRAIN.bmp");
	writer.WriteDestFile ();
}
*/

int g_totalRows = 0;
LARGE_INTEGER g_lastTime;
LARGE_INTEGER g_Freq;

void FormatTime(char *buffer, unsigned int bufferSize, double seconds)
{
	int min = (int)(seconds / 60.0);
	int rem_sec = (int)seconds % 60;
	if(min > 0)
		sprintf_s(buffer, bufferSize, "%d min %d sec", min, rem_sec);
	else
		sprintf_s(buffer, bufferSize, "%d sec", rem_sec);
}

void ShowMapBuildProgress(int row)
{
	LARGE_INTEGER currT;
	QueryPerformanceCounter(&currT);
	double diffT = (currT.QuadPart - g_lastTime.QuadPart) / (double)g_Freq.QuadPart;	// time taken to complete doing the previous line (in seconds)
	int rem_ops = g_totalRows - row;	// remaining operations to perform.
	double rem_estimate = diffT * rem_ops;
	static double prev_rem_estimate = rem_estimate;
	if (rem_estimate < prev_rem_estimate)
		prev_rem_estimate = rem_estimate;
	char buffer[32];
	FormatTime(buffer, 32, prev_rem_estimate);
	printf("%10d\t%10.2f%%\t%12s    \r",
		row, row * 100.0f / g_totalRows, buffer);
	g_lastTime = currT;
}

void GenrateMountainFlatTerrain(NoiseMap &heightMap, int heightmapWidth, int heightmapHeight)
{
	RidgedMulti	mountainTerrain;		//Mountain Modifier.
	ScaleBias	mountainScaler;			//
	Billow		baseFlatTerrain;			//The Base Flat Terrain.
	Perlin		terrainType;				//The Flat Terrain.
	Select		finalTerrain;				//The Final Output of the Terain.
	ScaleBias	flatTerrain;			//Modifier for scaling.

	mountainScaler.SetSourceModule( 0, mountainTerrain );
	mountainScaler.SetScale ( 1 );
	mountainScaler.SetBias(0);

	mountainTerrain.SetSeed(1);		//Mountain Modifier.
	mountainTerrain.SetOctaveCount( 3 );
	baseFlatTerrain.SetSeed(4);			//The Base Flat Terrain.
	terrainType.SetSeed(3);				//The Flat Terrain.

	baseFlatTerrain.SetFrequency (2.0);

	flatTerrain.SetSourceModule ( 0, baseFlatTerrain );
	flatTerrain.SetScale ( 0.0500 );
	flatTerrain.SetBias ( -0.95 );


	terrainType.SetFrequency (0.6);
	terrainType.SetPersistence (0.6);

	finalTerrain.SetSourceModule ( 0 , flatTerrain );
	finalTerrain.SetSourceModule ( 1 , mountainScaler );
	finalTerrain.SetControlModule( terrainType );
	finalTerrain.SetBounds ( 0.6, 1000.0 );
	finalTerrain.SetEdgeFalloff ( 0.525 );

	NoiseMapBuilderPlane heightMapBuilder;

	heightMapBuilder.SetSourceModule ( finalTerrain );
	heightMapBuilder.SetDestNoiseMap ( heightMap );
	heightMapBuilder.SetDestSize (heightmapWidth, heightmapHeight);
	heightMapBuilder.SetBounds ( 6.0, 11.0, 3.0, 8.0 );
	heightMapBuilder.SetCallback(ShowMapBuildProgress);
	printf_s("Started building the heightmap...\n");
	printf_s("Processed Lines Overall Progress Time Left\n");
	printf_s("--------------- ---------------- ---------\n");
	g_totalRows = heightmapHeight;
	QueryPerformanceCounter(&g_lastTime);
	heightMapBuilder.Build ();
	printf_s("\n");
	printf_s("Finished building the heightmap\n");
}

/*
void saveHeightMapToMultiFile(NoiseMap &map, int patchResolution)
{
	PatchResolution *pPatchResolution = PatchResolution::Create(patchResolution);
	PatchVerticesMapping *pPatchVerticesMapping = PatchVerticesMapping::Create(pPatchResolution);

	int hPatchesCount = map.GetWidth() / patchResolution;	// horizontal patches count
	int vPatchesCount = map.GetHeight() / patchResolution;	// vertical patches count
	map.SetBorderValue(-1.0f);
	float step = 4.0f;
	float heightscale = 95.625;

	// alloc buffer to hold the conversion results.
	int verticesCount = pPatchVerticesMapping->GetVerticesCountPerPatch();
	TerrainVertex *pBuffer = new TerrainVertex[verticesCount];
	int currFile = 0;

	for (int vPatch = 0; vPatch < vPatchesCount; ++vPatch)
		for (int hPatch = 0; hPatch < hPatchesCount; ++hPatch)
		{
			// patch (vPatch, hPatch) selected.
			for (int row = 0; row <= patchResolution; ++row)		// <= to get the pixel from the neighbouing patch
				for (int col = 0; col <= patchResolution; ++col)	// <=
				{
					int pixel_y = vPatch * patchResolution + row;
					int pixel_x = hPatch * patchResolution + col;
					float y = map.GetValue(pixel_x, pixel_y);

					int index = (*pPatchVerticesMapping)(row, col);
					pBuffer[index].x = pixel_x * step;
					pBuffer[index].z = pixel_y * step;
					pBuffer[index].y = y * heightscale;
					int color = (int)(127.5f * (y+1));
					color = clip(color, 0, 255);
					// color = D3DCOLOR_ARGB(255, color, color, color);	// height color
					color = D3DCOLOR_ARGB(255, 255, 255, 255);	// white color
					pBuffer[index].c = color;
				}

			// save the buffer for the selected patch.
			FILE* pfile;
			char filename[64];
			sprintf_s(filename, 64, "node_%d_%d", vPatch, hPatch);
			fopen_s(&pfile, filename, "wb");
			fwrite(pBuffer, verticesCount, sizeof(TerrainVertex), pfile);
			fclose(pfile);

			printf("%d\n", ++currFile);
		}


	delete [] pBuffer;
	delete pPatchVerticesMapping;
	delete pPatchResolution;
}
*/

void saveHeightMapToSingleFile(NoiseMap &map, int patchResolution, float stepX, float stepZ, float heightScale)
{
	PatchResolution *pPatchResolution = PatchResolution::Create(patchResolution);
	PatchVerticesMapping *pPatchVerticesMapping = PatchVerticesMapping::Create(pPatchResolution);

	int hPatchesCount = map.GetWidth() / patchResolution;	// horizontal patches count
	int vPatchesCount = map.GetHeight() / patchResolution;	// vertical patches count
	map.SetBorderValue(-1.0f);

	// alloc buffer to hold the conversion results.
	int verticesCount = pPatchVerticesMapping->GetVerticesCountPerPatch();
	float *patchHeights = new float[verticesCount];
	int currPatch = 0;

	// open the file for saving...
	FILE* pfile;
	fopen_s(&pfile, "map", "wb");

	int totalPatches = vPatchesCount * hPatchesCount;
	printf_s("Total Patches Count = %d\n", totalPatches);
	printf_s("Current Patch Overall Progress\n");
	printf_s("------------- ----------------\n");

	int vPatch, hPatch, row, col, pixel_x, pixel_y, index;
	for (vPatch = 0; vPatch < vPatchesCount; ++vPatch)
		for (hPatch = 0; hPatch < hPatchesCount; ++hPatch)
		{
			// patch (vPatch, hPatch) selected.
			for (row = 0; row <= patchResolution; ++row)		// "<=" to get the pixel from the bottom neighbouing patch
				for (col = 0; col <= patchResolution; ++col)	// "<=" to get the pixel from the right neighbouing patch
				{
					pixel_y = vPatch * patchResolution + row;
					pixel_x = hPatch * patchResolution + col;
					index = (*pPatchVerticesMapping)(row, col);
					patchHeights[index] = map.GetValue(pixel_x, pixel_y);
				}
			
			fwrite(patchHeights, verticesCount, sizeof(float), pfile);
			printf_s("%10d\t%10.2f%%\r", currPatch++, currPatch * 100.0f / totalPatches);
		}

	printf_s("\n");
	fclose(pfile);
	delete [] patchHeights;
	delete pPatchVerticesMapping;
	delete pPatchResolution;
}

void WriteHeightMapImageFile(NoiseMap &map)
{
	RendererImage renderer;
	Image image;
	renderer.SetSourceNoiseMap (map);
	renderer.SetDestImage (image);

	printf_s("Started rendering the HeightMap...\n");
	renderer.Render ();
	printf_s("Finished rendering the HeightMap\n");

	WriterBMP writer;
	writer.SetSourceImage (image);
	writer.SetDestFilename ("HeightMap.bmp");
	printf_s("Writing HeightMap.bmp...\n");
	writer.WriteDestFile ();
	printf_s("Finished writing HeightMap.bmp\n");
}

void GenerateTextrueFile(NoiseMap &map, int patchResolution, int &maxImageSize)
{
	RendererImage renderer;
	Image image;

	renderer.SetSourceNoiseMap (map);
	renderer.SetDestImage (image);

	renderer.ClearGradient ();
	renderer.AddGradientPoint (-1.00, Color ( 32, 160,   0, 255)); // grass
	renderer.AddGradientPoint (-0.25, Color (224, 224,   0, 255)); // dirt
	renderer.AddGradientPoint ( 0.25, Color (128, 128, 128, 255)); // rock
	renderer.AddGradientPoint ( 1.00, Color (255, 255, 255, 255)); // snow
	renderer.EnableLight ();
	renderer.SetLightContrast (2.0);
	renderer.SetLightBrightness (2.0);

	printf_s("Started rendering the Texture...\n");
	renderer.Render ();
	printf_s("Finished rendering the Texture\n");

	WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename("Texture.bmp");
	printf_s("Writing Texture.bmp...\n");
	writer.WriteDestFile ();
	printf_s("Finished writing Texture.bmp\n");

	int hPatchesCount = map.GetWidth() / patchResolution;	// horizontal patches count
	int vPatchesCount = map.GetHeight() / patchResolution;	// vertical patches count

	// open the file for saving...
	FILE *pfile, *imgFile;

	fopen_s(&pfile, "textures", "wb");
	// reserve the start of the file for the offsets & lengths of each patch file
	memset(imgBuffer, 0, MAX_TEXTURE_FILE_SIZE);
	assert(2 * sizeof(int) * vPatchesCount * hPatchesCount < MAX_TEXTURE_FILE_SIZE);
	fwrite(imgBuffer, 2 * sizeof(int), vPatchesCount * hPatchesCount, pfile);

	int imgOffset, imgSize;

	printf("Storing patches texture images...\n");
	int totalPatches = vPatchesCount * hPatchesCount;
	printf_s("Current Patch Overall Progress\n");
	printf_s("------------- ----------------\n");

	CImage img;
	img.Create(patchResolution, patchResolution, 24);

	for (int vPatch = 0; vPatch < vPatchesCount; ++vPatch)
		for (int hPatch = 0; hPatch < hPatchesCount; ++hPatch)
		{
			// patch (vPatch, hPatch) selected.
			for (int y = 0; y < patchResolution; ++y)		// <= to get the pixel from the neighbouing patch
				for (int x = 0; x < patchResolution; ++x)	// <=
				{
					int pixel_y = vPatch * patchResolution + y;
					int pixel_x = hPatch * patchResolution + x;
					Color c = image.GetValue(pixel_x, pixel_y);
					img.SetPixel(x, y, RGB(c.red, c.green, c.blue));
				}

			fseek(pfile, 0, SEEK_END);
			imgOffset = ftell(pfile);

			img.Save("img", Gdiplus::ImageFormatJPEG);
			// open the file and paste it within the "textures" file
			fopen_s(&imgFile, "img", "rb");
			fseek(imgFile, 0, SEEK_END);
			imgSize = ftell(imgFile);	// size of the file
			assert(imgSize < MAX_TEXTURE_FILE_SIZE);	// currently file size musn't exceed 1 mega size limit.
			if (imgSize > maxImageSize)
				maxImageSize = imgSize;
			fseek(imgFile, 0, SEEK_SET);
			fread_s(imgBuffer, MAX_TEXTURE_FILE_SIZE, 1, imgSize, imgFile);	// read the image into the imgBuffer.
			fclose(imgFile);

			// textures file pointer is at the end of the file.
			fwrite(imgBuffer, 1, imgSize, pfile);

			// register the texture offset and size in the header of the textures file.
			int patchIndex = vPatch * hPatchesCount + hPatch;
			fseek(pfile, patchIndex * 2 * sizeof(int), SEEK_SET);

			fwrite(&imgOffset, sizeof(int), 1, pfile);
			fwrite(&imgSize, sizeof(int), 1, pfile);

			printf_s("%10d\t%10.2f%%\r", patchIndex, patchIndex * 100.0f / totalPatches);
		}

	printf("Finished Storing patches texture images.\n");

	img.Destroy();
	fclose(pfile);
	assert(DeleteFile("img"));
}

void WriteMajorFile(int patchResolution, int hPatchesCount, int vPatchesCount, float stepX, float stepZ, float heightScale, int maxImageSize)
{
	// read patch resolution
	FILE* pfile = NULL;
	fopen_s(&pfile, "major", "wb");
	if (NULL == pfile) return;
	fwrite(&patchResolution, sizeof(int), 1, pfile);
	fwrite(&hPatchesCount, sizeof(int), 1, pfile);
	fwrite(&vPatchesCount, sizeof(int), 1, pfile);
	fwrite(&stepX, sizeof(float), 1, pfile);
	fwrite(&stepZ, sizeof(float), 1, pfile);
	fwrite(&heightScale, sizeof(float), 1, pfile);
	fwrite(&maxImageSize, sizeof(int), 1, pfile);
	fclose(pfile);
}

int _tmain(int argc, _TCHAR* argv[])
{
//	GenrateNormalTerrain();
//	GenrateValiesTerrain();
	LARGE_INTEGER start, end;
	SetThreadAffinityMask(GetCurrentThread(), 1);
	QueryPerformanceFrequency(&g_Freq);
	QueryPerformanceCounter(&start);

	NoiseMap heightMap;

	int heightmapWidth = 4096;
	int heightmapHeight = 4096;
	int patchResolution = 32;
	float stepX = 4.0f;
	float stepZ = 4.0f;
	float heightScale = 95.625f;

	int hPatchesCount = heightmapWidth / patchResolution;
	int vPatchesCount = heightmapHeight / patchResolution;
	int maxImageSize = 0;

	printf_s("HeightMap (W = %d, H = %d)\n", heightmapWidth, heightmapHeight);
	GenrateMountainFlatTerrain(heightMap, heightmapWidth, heightmapHeight);
	// Convert this height map into patches and save them directly to the hard disk.
	printf_s("Writing patches into the map file...\n");
	saveHeightMapToSingleFile(heightMap, patchResolution, stepX, stepZ, heightScale);
	printf_s("Finished writing patches into the map file.\n");
	WriteHeightMapImageFile(heightMap);
	GenerateTextrueFile(heightMap, patchResolution, maxImageSize);
	printf_s("Writing major file...\n");
	WriteMajorFile(patchResolution, hPatchesCount, vPatchesCount, stepX, stepZ, heightScale, maxImageSize);
	printf_s("Finished writing major file.\n");
	QueryPerformanceCounter(&end);
	char buffer[32];
	FormatTime(buffer, 32, (end.QuadPart - start.QuadPart)/(double)g_Freq.QuadPart);
	printf("Overall time taken: %s\n", buffer);
	printf("Press any key to continue...\n");
	getchar();

	return 0;
}