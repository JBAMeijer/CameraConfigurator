#pragma once
#include <cstdint>
#include "config.h"

struct CameraProperties
{
	float pixelSize;

	float sensorDiagonal;
	float sensorWidth;
	float sensorHeight;

	int pixelWidth;
	int pixelHeight;
};

struct Camera
{
    uint8_t itemsStored = 0;
    char name[MAX_ITEMS_ALLOWED][CAMERA_NAME_STRING_SIZE];
	CameraProperties cProps[MAX_ITEMS_ALLOWED];
};

struct AppState
{
    bool cameraModalEnabled = false;

    Camera cameras;
};