#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "state.h"


#define AMOUNT_VARIABLES 6
int read_camera_properties(const char* filename, Camera* cameras)
{
    uint8_t current_variable = 0;
    uint8_t current_camera = 0;

    char buffer[200];
    FILE* handle = fopen(filename, "r");
    if (handle == NULL)
    {
        return -2;
    }

    while (fgets(buffer, sizeof(buffer), handle) != NULL)
    {
        if (buffer[0] == '\n')
            continue;

        switch (current_variable) {
        case 0: memcpy(cameras->name[current_camera], buffer + (sizeof("cameraName=") - 1), CAMERA_NAME_STRING_SIZE); break;
        case 1: cameras->cProps[current_camera].pixelSize = strtof(buffer + (sizeof("pixelSize=") - 1), NULL); break;
        case 2: cameras->cProps[current_camera].sensorDiagonal = strtof(buffer + (sizeof("sensorDiagonal=") - 1), NULL); break;
        case 3: cameras->cProps[current_camera].sensorWidth = strtof(buffer + (sizeof("sensorWidth=") - 1), NULL); break;
        case 4: cameras->cProps[current_camera].sensorHeight = strtof(buffer + (sizeof("sensorHeight=") - 1), NULL); break;
        case 5: cameras->cProps[current_camera].pixelWidth = strtol(buffer + (sizeof("pixelWidth=") - 1), NULL, 10); break;
        case 6: cameras->cProps[current_camera].pixelHeight = strtol(buffer + (sizeof("pixelHeight=") - 1), NULL, 10); break;
        }

        if (AMOUNT_VARIABLES != current_variable) {
            current_variable++;
        }
        else {
            current_variable = 0;
            current_camera++;
        }
    }

	cameras->itemsStored = current_camera;
    fclose(handle);

    return 0;
}

int write_camera_properties(const char* filename, Camera* cameras)
{
    char buffer[200];

    FILE* handle = fopen(filename, "w");
    if (handle == NULL)
    {
        return -1;
    }

    for (int i = 0; i < cameras->itemsStored; i++)
    {
        sprintf(buffer, "%s%s", "cameraName=", cameras->name[i]);
        fputs(buffer, handle);

        sprintf(buffer, "%s%f\n", "pixelSize=", cameras->cProps[i].pixelSize);
        fputs(buffer, handle);

        sprintf(buffer, "%s%f\n", "sensorDiagonal=", cameras->cProps[i].sensorDiagonal);
        fputs(buffer, handle);

        sprintf(buffer, "%s%f\n", "sensorWidth=", cameras->cProps[i].sensorWidth);
        fputs(buffer, handle);

        sprintf(buffer, "%s%f\n", "sensorHeight=", cameras->cProps[i].sensorHeight);
        fputs(buffer, handle);

        sprintf(buffer, "%s%d\n", "pixelWidth=", cameras->cProps[i].pixelWidth);
        fputs(buffer, handle);

        if (i != cameras->itemsStored - 1) {
            sprintf(buffer, "%s%d\n\n", "pixelHeight=", cameras->cProps[i].pixelHeight);
            fputs(buffer, handle);
        }
        else {
            sprintf(buffer, "%s%d", "pixelHeight=", cameras->cProps[i].pixelHeight);
            fputs(buffer, handle);
        }
    }

    fclose(handle);
    return 0;
}

void DeleteCamera(Camera* cameras, uint8_t delete_index)
{
	if (cameras->itemsStored - 1 == delete_index) {
		cameras->itemsStored--;
	}
	else {
		for (uint8_t i = delete_index + 1; i < cameras->itemsStored; i++)
		{
			memcpy(cameras->name[i - 1], cameras->name[i], CAMERA_NAME_STRING_SIZE);
			cameras->cProps[i - 1] = cameras->cProps[i];
		}
		cameras->itemsStored--;
	}

	write_camera_properties("cameras.txt", cameras);
}

void AddOrUpdateCamera(Camera* cameras, const char* cameraName, CameraProperties *cProps)
{
	for (uint8_t i = 0; i < cameras->itemsStored; i++)
	{
		if (strcmp(cameras->name[i], cameraName) == 0)
		{
			cameras->cProps[i] = *cProps;
			write_camera_properties("cameras.txt", cameras);
			return;
		}
	}

	memcpy(cameras->name[cameras->itemsStored], cameraName, CAMERA_NAME_STRING_SIZE);
	memcpy(&cameras->cProps[cameras->itemsStored], cProps, sizeof(CameraProperties));
	cameras->itemsStored++;

	write_camera_properties("cameras.txt", cameras);
}