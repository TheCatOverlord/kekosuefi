#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef enum {BLACK, WHITE, RED, GREEN, BLUE} Color;

void ClearFramebuffer(FrameBuffer* frameBuffer)
{
	unsigned int BBP = 4;
	for (unsigned int i = 0; i < frameBuffer->bufferSize; i+=BBP)
	{
		*(unsigned int*)(i + frameBuffer->baseAddress) = 0xff000000;
	}	
	return;
}

void PutPixel(FrameBuffer* frameBuffer, unsigned int x, unsigned int y, Color color)
{
	unsigned int BBP = 4;
	switch (color)
	{
		case BLACK:
		{ *(unsigned int*)(x+(y*frameBuffer->pixelsPerScanline * BBP) + frameBuffer->baseAddress) = 0xff000000; } break;
		case WHITE:
		{ *(unsigned int*)(x+(y*frameBuffer->pixelsPerScanline * BBP) + frameBuffer->baseAddress) = 0xffffffff; } break;
		case RED:
		{ *(unsigned int*)(x+(y*frameBuffer->pixelsPerScanline * BBP) + frameBuffer->baseAddress) = 0xff0000ff; } break;
		case GREEN:
		{ *(unsigned int*)(x+(y*frameBuffer->pixelsPerScanline * BBP) + frameBuffer->baseAddress) = 0xff00ff00; } break;
		case BLUE:
		{ *(unsigned int*)(x+(y*frameBuffer->pixelsPerScanline * BBP) + frameBuffer->baseAddress) = 0xffff0000; } break;

		default: break;
	}
	return;
}

unsigned int lerp(unsigned int start, unsigned int end, unsigned int value,unsigned int steps)
{ return ((end - start) * (value / steps)) + start; }


void DrawRainbowLine(FrameBuffer* frameBuffer, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	unsigned int steps = 64;
	unsigned int colorIndex = 0;
	for (unsigned int t = 0; t < steps; t++)
	{
		unsigned int x, y;
		x = lerp(x1, x2, t, steps);
		y = lerp(y1, y2, t, steps);
		switch (colorIndex)
		{
			case 0:
			{
				PutPixel(frameBuffer, x, y, RED);
			} break;

			case 1:
			{
				PutPixel(frameBuffer, x, y, GREEN);
			} break;

			case 2:
			{
				PutPixel(frameBuffer, x, y, BLUE);
			} break;

			default: break;
		}

		colorIndex++;
		if (colorIndex > 2) { colorIndex = 0; }
	}
	return;
}

#endif