#pragma once
#include "efiMemory.h"
#include <stdint.h>
#include "Bitmap.h"
#include "memory.h"

class PageFrameAllocator
{
    public:
        void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize);
        Bitmap PageBitmap;
        void FreePage(void* address);
        void FreePages(void* address, uint64_t pageCount);
        void LockPage(void* address);
        void LockPages(void* address, uint64_t pageCount);
        void* RequestPage();
        uint64_t GetFreeRam();
        uint64_t GetUsedRam();
        uint64_t GetReservedRam();



    private:
        void InitBitmap(size_t bitmapSize, void* bufferAddr);
        void ReservePage(void* address);
        void ReservePages(void* address, uint64_t pageCount);
        void UnreservePage(void* address);
        void UnreservePages(void* address, uint64_t pageCount);
};