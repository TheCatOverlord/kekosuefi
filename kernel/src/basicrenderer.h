#pragma once
#include "math.h"
#include "framebuffer.h"
#include "simplefont.h"

class BasicRenderer
{
    public:
        BasicRenderer(FrameBuffer* Framebuffer, PSF1_FONT* Font);

        Point cursorPos;
        FrameBuffer* frameBuffer;
        PSF1_FONT* font;
        unsigned int color;

        void ReturnCursor();
        void PutChar(char chr, unsigned int xOff, unsigned int yOff);
        void Print(const char* str);


        void Clear(unsigned int color);
        void DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
        unsigned int lerp(unsigned int start, unsigned int end, unsigned int value, unsigned int steps);
        void PutPixel(unsigned int x, unsigned int y);

    private:

};