#include <stddef.h>
#include "basicrenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "Bitmap.h"
#include "PageFrameAllocator.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct BootInfo
{
	FrameBuffer* frameBuffer;
	PSF1_FONT* font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
};

void TestPrintEFIMemoryMap(BasicRenderer newRenderer, BootInfo* bootInfo)
{
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;
    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)bootInfo->mMap + (i * bootInfo->mMapDescSize));
        newRenderer.ReturnCursor();
        newRenderer.Print(EFI_MEMORY_TYPE_STRINGS[desc->type]);
        newRenderer.color = 0xffff00ff;
        newRenderer.Print(" ");
        newRenderer.Print(to_str(desc->numPages * 4069 / 1024));
        newRenderer.Print(" ");
        newRenderer.Print("KB");
        newRenderer.color = 0xffffffff;
    }
    return;
}

void TestPrintNumbers(BasicRenderer newRenderer)
{
    newRenderer.Print("[INFO] Test printing numbers...");
    newRenderer.ReturnCursor();
    newRenderer.Print("[INFO] unsigned 64bit int: ");
    newRenderer.Print(to_str((uint64_t)420));
    newRenderer.ReturnCursor();
    newRenderer.Print("[INFO] signed 64bit int: ");
    newRenderer.Print(to_str((int64_t)-69));
    newRenderer.ReturnCursor();
    newRenderer.Print("[INFO] signed double: ");
    newRenderer.Print(to_str((double)-69.420));
    newRenderer.ReturnCursor();
    
    newRenderer.Print("[INFO] Printing 64 bit hex...");
    newRenderer.ReturnCursor();

    for (uint8_t i = 0; i < 16; i++)
    {
        newRenderer.Print(to_hstr((uint64_t)i));
        newRenderer.cursorPos.x += 16;
        i++;
        newRenderer.Print(to_hstr((uint64_t)i));
        newRenderer.ReturnCursor();   
    }

    newRenderer.Print("[INFO] Printing 32 bit hex...");
    newRenderer.ReturnCursor();

    for (uint8_t i = 0; i < 16; i++)
    {
        newRenderer.Print(to_hstr((uint32_t)i));
        newRenderer.cursorPos.x += 16;
        i++;
        newRenderer.Print(to_hstr((uint32_t)i));
        newRenderer.ReturnCursor();   
    }

    newRenderer.Print("[INFO] Printing 16 bit hex...");
    newRenderer.ReturnCursor();

    for (uint8_t i = 0; i < 16; i++)
    {
        newRenderer.Print(to_hstr((uint16_t)i));
        newRenderer.cursorPos.x += 16;
        i++;
        newRenderer.Print(to_hstr((uint16_t)i));
        newRenderer.ReturnCursor();   
    }

    newRenderer.Print("[INFO] Printing 8 bit hex...");
    newRenderer.ReturnCursor();

    for (uint8_t i = 0; i < 16; i++)
    {
        newRenderer.Print(to_hstr(i));
        newRenderer.cursorPos.x += 16;
        i++;
        newRenderer.Print(to_hstr(i));
        newRenderer.ReturnCursor();   
    }
    return;
}

void TestRequestPages(BasicRenderer* newRenderer, PageFrameAllocator* newAllocator)
{
    newRenderer->Print("[INFO] |------------------|");
    newRenderer->ReturnCursor();
    newRenderer->Print("[INFO] | Requesting pages |");
    newRenderer->ReturnCursor();
    newRenderer->Print("[INFO] |------------------|");
    newRenderer->ReturnCursor();

    for (int i = 0; i < 20; i++)
    {
        void* address = newAllocator->RequestPage();
        newRenderer->Print("[INFO] Page ");
        newRenderer->Print(to_str((uint64_t)i));
        newRenderer->Print(": ");
        newRenderer->Print(to_hstr((uint64_t)address));
        newRenderer->ReturnCursor();
    }
    return;
}

void InitPrintMemoryAmount(BasicRenderer* newRenderer, BootInfo* bootInfo, PageFrameAllocator newAllocator)
{
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;
    newRenderer->Print("[INFO] Amount of installed memory: ");
    uint64_t memAmount = GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize);
    uint64_t memAmountMB = memAmount / 1048576;
    newRenderer->Print(to_str(memAmountMB));
    newRenderer->Print(" MB");
    newRenderer->ReturnCursor();

    newRenderer->Print("[INFO] Free Ram: ");
    newRenderer->Print(to_str(newAllocator.GetFreeRam() / 1024));
    newRenderer->Print(" KB");
    newRenderer->ReturnCursor();
    newRenderer->Print("[INFO] Used Ram: ");
    newRenderer->Print(to_str(newAllocator.GetUsedRam() / 1024));
    newRenderer->Print(" KB");
    newRenderer->ReturnCursor();
    newRenderer->Print("[INFO] Reserved Ram: ");
    newRenderer->Print(to_str(newAllocator.GetReservedRam() / 1024));
    newRenderer->Print(" KB");
    newRenderer->ReturnCursor();
}

extern "C" void _start(BootInfo* bootInfo)
{
	BasicRenderer newRenderer(bootInfo->frameBuffer, bootInfo->font);
    PageFrameAllocator newAllocator;
    newAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);
    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
    newAllocator.LockPages(&_KernelStart, kernelPages);

    newRenderer.Clear(0xff000000);
    newRenderer.Print("  _  __    _    ___  ___ ");
    newRenderer.ReturnCursor();
    newRenderer.Print(" | |/ /___| |__/ _ \\/ __|");
    newRenderer.ReturnCursor();
    newRenderer.Print(" | ' </ -_) / / (_) \\__ \\");
    newRenderer.ReturnCursor();
    newRenderer.Print(" |_|\\_\\___|_\\_\\\\___/|___/");
    newRenderer.ReturnCursor();
    newRenderer.ReturnCursor();
    InitPrintMemoryAmount(&newRenderer, bootInfo, newAllocator);

    // TestPrintNumbers(newRenderer);
    // TestPrintEFIMemoryMap(newRenderer, bootInfo);
    // TestRequestPages(&newRenderer, &newAllocator)

    // unsigned int srcWidth, srcHeight;
    // srcWidth = newRenderer.frameBuffer->width;
    // srcHeight = newRenderer.frameBuffer->height;

    // unsigned int srcCenterx, srcCentery;

    // srcCenterx = srcWidth / 2;
    // srcCentery = srcHeight / 2;

    // unsigned int xtl, ytl, xtr, ytr, xbl, ybl, xbr, ybr;
    // unsigned int xbtl, ybtl, xbtr, ybtr, xbbl, ybbl, xbbr, ybbr;

    // unsigned int size = 100;

    // xtl = srcCenterx + (size * -0.5f);
    // ytl = srcCentery + (size * -0.5f);
    // xtr = srcCenterx + (size *  0.5f);
    // ytr = srcCentery + (size * -0.5f);
    // xbl = srcCenterx + (size * -0.5f);
    // ybl = srcCentery + (size *  0.5f);
    // xbr = srcCenterx + (size *  0.5f);
    // ybr = srcCentery + (size *  0.5f);

    // xbtl = srcCenterx - size;
    // ybtl = srcCentery - size;
    // xbtr = srcCenterx;
    // ybtr = srcCentery - size;
    // xbbl = srcCenterx - size;
    // ybbl = srcCentery;
    // xbbr = srcCenterx;
    // ybbr = srcCentery;

    // newRenderer.DrawLine(xtl, ytl, xtr, ytr);
    // newRenderer.DrawLine(xtl, ytl, xbl, ybl);
    // newRenderer.DrawLine(xtr, ytr, xbr, ybr);
    // newRenderer.DrawLine(xbl, ybl, xbr, ybr);

    // newRenderer.DrawLine(xbtl, ybtl, xbtr, ybtr);
    // newRenderer.DrawLine(xbtl, ybtl, xbbl, ybbl);
    // newRenderer.DrawLine(xbtr, ybtr, xbbr, ybbr);
    // newRenderer.DrawLine(xbbl, ybbl, xbbr, ybbr);
    
    // newRenderer.DrawLine(xtl, ytl, xbtl, ybtl);
    // newRenderer.DrawLine(xtr, ytr, xbtr, ybtr);
    // newRenderer.DrawLine(xbl, ybl, xbbl, ybbl);
    // newRenderer.DrawLine(xbr, ybr, xbbr, ybbr);
    
    return;
}