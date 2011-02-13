#include "dxstdafx.h"
#include "WorkingSetWindow.h"
#include "TerrainPager.h"
#include "BaseCamera.h"

void WorkingSetWindow::SetCamera(BaseCamera *pCamera)
{
	if (this->pCamera== pCamera)
		return;

	if (NULL != this->pCamera)
		this->pCamera->RemoveEventListener(this);

	this->pCamera = pCamera;

	if (NULL != pCamera)
		pCamera->AddEventListener(this);
}

BaseCamera * WorkingSetWindow::GetCamera() const
{
	return pCamera;
}

WorkingSetWindow::WorkingSetWindow(TerrainPager &terrainPager)
: terrainPager(terrainPager)
{
	pCamera = NULL;
}