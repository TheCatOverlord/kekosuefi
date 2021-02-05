#pragma once
#include <stddef.h>

struct  FrameBuffer
{
	void * baseAddress;
	size_t bufferSize;
	unsigned int width;
	unsigned int height;
	unsigned int pixelsPerScanline;
};