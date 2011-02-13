#pragma once

#include "CameraEventListener.h"

class Patch;
class TerrainPager;
class BaseCamera;

class WorkingSetWindow : CameraEventListener
{
private:
	BaseCamera *pCamera;
	int priority;

protected:
	TerrainPager &terrainPager;

public:
	virtual void SetCamera(BaseCamera *pCamera);
	BaseCamera * GetCamera() const;
	virtual unsigned int GetNbContainedPatches() const = 0;

private:
	WorkingSetWindow & operator= (WorkingSetWindow &) {}

public:
	WorkingSetWindow(TerrainPager &terrainPager);
};