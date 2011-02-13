#pragma once

#include "BaseCamera.h"

class CameraEventListener
{
public:
	virtual void OnCameraEvent(class BaseCamera &camera, CAMERA_EVENT_LIST &eventList) {}
};