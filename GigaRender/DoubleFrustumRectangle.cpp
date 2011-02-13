#include "dxstdafx.h"
#include "DoubleFrustumRectangle.h"
#include "TerrainPager.h"

void DoubleFrustumRectangle::SetCamera(BaseCamera *pCamera)
{
	WorkingSetWindow::SetCamera(pCamera);

	if (NULL == pCamera)	// discuss what to do in this case.
	{
		updateWorkingSet(0, 0, 0, 0);	// make the window closes all the patches related to it.
		return;
	}


	// update window dimensions
	// get frustum height.
	float frustumHeight = pCamera->GetFarClip();
	float halfFOV = 0.5f * pCamera->GetFOV();
	float hypotenuseLength = frustumHeight / cosf(halfFOV);

	float windowHeight = 2 * hypotenuseLength;
	float windowWidth = windowHeight * ratioWidthToHeight;

	const D3DXVECTOR3 &vEye = pCamera->GetEyePt();
	float x = vEye.x - 0.5f * windowWidth;
	float z = vEye.z - 0.5f * windowHeight;

	updateWorkingSet(x, z, (int)ceil(windowWidth / patchWidth), (int)ceil(windowHeight / patchWidth));
}

void DoubleFrustumRectangle::updateWorkingSet(float x, float z, int new_hPatches, int new_vPatches)
{
	int new_idI, new_idJ;

	float idJX = patchID.GetJ() * patchWidth;
	float idIZ = patchID.GetI() * patchWidth;

	if (idJX - x > 0.75f * patchWidth)
		new_idJ = (int)floorf(x / patchWidth);
	else if (x - idJX > 0.75f * patchWidth)
		new_idJ = (int)ceilf(x / patchWidth);
	else
		new_idJ = patchID.GetJ();

	if (idIZ - z > 0.75f * patchWidth)
		new_idI = (int)floorf(z / patchWidth);
	else if (z - idIZ > 0.75f * patchWidth)
		new_idI = (int)ceilf(z / patchWidth);
	else
		new_idI = patchID.GetI();

	if (new_idI == patchID.GetI() &&
		new_idJ == patchID.GetJ() &&
		new_vPatches == vPatchesCount &&
		new_hPatches == hPatchesCount)	// short cut.	// not correct what if only the width & height of the window changed.
		return;

	int i, j;

	// unload patches that from the old working set that currently don't belong to the new working set.
	for (i = patchID.GetI(); i < patchID.GetI() + vPatchesCount; ++i)
		for (j = patchID.GetJ(); j < patchID.GetJ() + hPatchesCount; ++j)
			if (i < new_idI || i >= new_idI + new_vPatches ||
				j < new_idJ || j >= new_idJ + new_hPatches)
				terrainPager.FreePatch(PatchID(i, j));

	// now load patches in the new working set which are not loaded yet.
	for (i = new_idI; i < new_idI + new_vPatches; ++i)
		for (j = new_idJ; j < new_idJ + new_hPatches; ++j)
			if (i < patchID.GetI() || i >= patchID.GetI() + vPatchesCount ||
				j < patchID.GetJ() || j >= patchID.GetJ() + hPatchesCount)
				terrainPager.GetPatch(PatchID(i, j));

	patchID = PatchID(new_idI, new_idJ);	
	vPatchesCount = new_vPatches;
	hPatchesCount = new_hPatches;
}

unsigned int DoubleFrustumRectangle::GetNbContainedPatches() const
{
	return hPatchesCount * vPatchesCount;
}

void DoubleFrustumRectangle::OnCameraEvent(class BaseCamera &camera, CAMERA_EVENT_LIST &eventList)
{
	// in double frustum rectangle window algorithm we don't care orientation changes.
	// so check if the position changed or not.
	CAMERA_EVENT_LIST::iterator t, e = eventList.end();
	for(t = eventList.begin(); t != e; ++t)
	{
		if (CAMERA_POSITION_CHANGED == *t)
		{
			const D3DXVECTOR3 &vEye = camera.GetEyePt();
			// determine if (idI, idJ) changed after the window position change.
			float x = vEye.x - 0.5f * /*windowWidth*/ hPatchesCount * patchWidth;
			float z = vEye.z - 0.5f * /*windowHeight*/ vPatchesCount * patchWidth;

			updateWorkingSet(x, z, hPatchesCount, vPatchesCount);
		}
		else if (CAMERA_PROJECTION_PARAMETERS_CHANGED == *t)
		{
			// update window dimensions
		}
	}
}

DoubleFrustumRectangle::DoubleFrustumRectangle(TerrainPager &terrainPager, float ratioWidthToHeight)
: WorkingSetWindow(terrainPager), ratioWidthToHeight(ratioWidthToHeight)
{
	vPatchesCount = hPatchesCount = 0;	// initalize local variables.
	patchWidth = terrainPager.GetPatchWidth();	// save terrain patch width.
}