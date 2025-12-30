#pragma once

#include <opencv2/imgcodecs.hpp>

uint8_t* data;
cv::Mat image;

int InitHyperSpectralViewer(const char* filename, int width, int height, int channels)
{
	char buffer[200] = {0};

	int totalbytes = width * height * channels;
	data = (uint8_t*)malloc(totalbytes);

	FILE* handle = fopen(filename, "w");
	if (handle == NULL)
	{
		fprintf(stderr, "Failed to create new file!\n");
		return -1;
	}

	image = cv::Mat(cv::Size(width, height), CV_8U, data);
}

void HyperSpectralImages(AppState* state)
{

}

