#include "PageFrameAllocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initalized = false;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize)
{
    if(Initalized) { return; }
    Initalized = true;

    uint64_t mMapEntries = mMapSize / mMapDescSize;

    void* largestFreeMemSeg = NULL;
    size_t largestFreeMemSegSize = 0;

    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        if (desc->type == 7)
        {
            if (desc->numPages * 4096 > largestFreeMemSegSize)
            {
                largestFreeMemSeg = desc->physAddr;
                largestFreeMemSegSize = desc->numPages * 4096;
            }
        }
    }
    uint64_t memorySize = GetMemorySize(mMap, mMapEntries, mMapDescSize);
    freeMemory = memorySize;
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;

    InitBitmap(bitmapSize, largestFreeMemSeg);

    LockPages(&PageBitmap, PageBitmap.size / 4096 + 1);

    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc =  (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        if (desc->type != 7)
        {
            ReservePages(desc->physAddr, desc->numPages);
        }
    }

}

void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddr)
{
    PageBitmap.size = bitmapSize;
    PageBitmap.buffer = (uint8_t*)bufferAddr;
    for (int i = 0; i < bitmapSize; i++)
    {
        *(uint8_t*)(PageBitmap.buffer + i) = 0;
    }
}

void PageFrameAllocator::FreePage(void* address)
{
    uint64_t index = (uint64_t) address / 4096;
    if(PageBitmap[index] == false) return;
    PageBitmap.Set(index, false);
    freeMemory += 4096;
    usedMemory -= 4096;
}

void PageFrameAllocator::LockPage(void* address)
{
    uint64_t index = (uint64_t) address / 4096;
    if(PageBitmap[index] == true) return;
    PageBitmap.Set(index, true);
    freeMemory -= 4096;
    usedMemory += 4096;
}

void PageFrameAllocator::ReservePage(void* address)
{
    uint64_t index = (uint64_t) address / 4096;
    if(PageBitmap[index] == true) return;
    PageBitmap.Set(index, true);
    freeMemory -= 4096;
    reservedMemory += 4096;
}

void PageFrameAllocator::UnreservePage(void* address)
{
    uint64_t index = (uint64_t) address / 4096;
    if(PageBitmap[index] == true) return;
    PageBitmap.Set(index, true);
    freeMemory -= 4096;
    reservedMemory += 4096;
}

void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount)
{
    for (int t = 0; t < pageCount; t++)
    {
        ReservePage((void*)((uint64_t)address + (t*4096)));
    }
}

void PageFrameAllocator::UnreservePages(void* address, uint64_t pageCount)
{
    for (int t = 0; t < pageCount; t++)
    {
        UnreservePage((void*)((uint64_t)address + (t*4096)));
    }
}

void PageFrameAllocator::FreePages(void* address, uint64_t pageCount)
{
    for (int t = 0; t < pageCount; t++)
    {
        FreePage((void*)((uint64_t)address + (t*4096)));
    }
}

void PageFrameAllocator::LockPages(void* address, uint64_t pageCount)
{
    for (int t = 0; t < pageCount; t++)
    {
        LockPage((void*)((uint64_t)address + (t*4096)));
    }
}

void* PageFrameAllocator::RequestPage()
{
    for (uint64_t index = 0; index < PageBitmap.size * 8; index++)
    {
        if (PageBitmap[index] == true) continue;
        LockPage((void*)(index * 4096));
        return (void*)(index * 4096);
    }

    return NULL; // Page Frame swap to file
}

uint64_t PageFrameAllocator::GetFreeRam()
{ return freeMemory; }
uint64_t PageFrameAllocator::GetUsedRam()
{ return usedMemory; }
uint64_t PageFrameAllocator::GetReservedRam()
{ return reservedMemory; }