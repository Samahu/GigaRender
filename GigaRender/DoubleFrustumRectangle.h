#pragma once

#include "WorkingSetWindow.h"
#include "PatchID.h"

class DoubleFrustumRectangle : public WorkingSetWindow
{
private:
	float patchWidth;
	float ratioWidthToHeight;

	int vPatchesCount;			// patches count over z-coordinates
	int hPatchesCount;			// patches count over x-coordinates

	PatchID		patchID;
	// patchID.GetI() is related to the z-coordinates (rows)
	// patchID.GetJ() is related to the x-coordinates (cols)

public:
	virtual void SetCamera(BaseCamera *pCamera);		// override
	virtual unsigned int GetNbContainedPatches() const;	// override

private:
	void DoubleFrustumRectangle::updateWorkingSet(float x, float z, int new_hPatches, int new_vPatches);

protected:
	virtual void OnCameraEvent(class BaseCamera &camera, CAMERA_EVENT_LIST &eventList);	// override

public:
	DoubleFrustumRectangle(TerrainPager &terrainPager, float ratioWidthToHeight = 1.0f);
};