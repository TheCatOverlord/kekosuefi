#include "basicrenderer.h"

BasicRenderer::BasicRenderer(FrameBuffer* Framebuffer, PSF1_FONT* Font)
{
    frameBuffer = Framebuffer;
    font = Font;
    color = 0xffffffff;
    cursorPos = { 0, 0 };
}

void BasicRenderer::ReturnCursor()
{
	cursorPos = {0, cursorPos.y + 16};
	return;
}

// Function to write a character to a framebuffer
void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
{
	unsigned int* pixPtr = (unsigned int*)frameBuffer->baseAddress;
	char* fontPtr = (char*)font->glyphbuffer + (chr * font->psf1_header->charsize);
	for (unsigned long y = yOff; y < yOff + 16; y++ )
	{
		for (unsigned long x = xOff; x < xOff + 8; x++)
		{
			if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0) 
			{ *(unsigned int*)(pixPtr + x + (y * frameBuffer->pixelsPerScanline)) = color; }
		}
		fontPtr++;
	}
	return;
}

// Function to write a string to a framebuffer
void BasicRenderer::Print(const char* str)
{
	char* chr = (char*)str;
	while (*chr != 0)
	{
		PutChar(*chr, cursorPos.x, cursorPos.y);	
		chr++;
		if (cursorPos.x + 8 > frameBuffer->width)
		{
			cursorPos.y += 16;
			cursorPos.x = 0;
		}
		else
		{ cursorPos.x +=8; }
	}
}

void BasicRenderer::Clear(unsigned int clearColor)
{
	for (unsigned int i = 0; i < frameBuffer->bufferSize; i+=4)
	{
		*(unsigned int*)(i + (unsigned int*)frameBuffer->baseAddress) = clearColor;
	}
	return;
}

unsigned int BasicRenderer::lerp(unsigned int start, unsigned int end, unsigned int value,unsigned int steps)
{ return ((end - start) * ((float)value / (float)steps)) + start; }

void BasicRenderer::PutPixel(unsigned int x, unsigned int y)
{ *(unsigned int*)((x)+(y*frameBuffer->pixelsPerScanline) + (unsigned int*)frameBuffer->baseAddress) = color; }

void BasicRenderer::DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	unsigned int steps = 128;
	for (unsigned int i = 0;  i < steps;  i++)
	{
		unsigned int x, y;
		x = lerp(x1, x2, i, steps);
		y = lerp(y1, y2, i, steps);
		PutPixel(x, y);
	}
	return;
}