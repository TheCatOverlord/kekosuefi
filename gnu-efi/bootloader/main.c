#include <efi.h>
#include <efilib.h>
#include <elf.h>

/////////////
/* Defines */
/////////////

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

//////////////
/* Typedefs */
//////////////
typedef unsigned long long size_t; // define our own definition of a size_t

//////////////////
/* Data structs */
////////////////// 

// struct to define the framebuffer
typedef struct 
{
	void * baseAddress;
	size_t bufferSize;
	unsigned int width;
	unsigned int height;
	unsigned int pixelsPerScanline;
} FrameBuffer;

// struct to define the psf header
typedef struct 
{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

// struct to define the psf font
typedef struct
{
	PSF1_HEADER* psf1_header;
	void * glyphbuffer;
} PSF1_FONT;

/////////////////////
/* Global varibles */
/////////////////////

// a global varible for the framebuffer
FrameBuffer frameBuffer;

// Function to load an efi file
EFI_FILE* LoadFile(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	EFI_FILE* loadedFile;
	EFI_LOADED_IMAGE_PROTOCOL* loadedImage;

	// Get the loaded image
	systemTable->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystem;

	// Get the loaded file system
	systemTable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&fileSystem);

	// open the directory if it isnt open already
	if (directory == NULL)
	{
		fileSystem->OpenVolume(fileSystem, &directory);
	}

	// Opens the file
	EFI_STATUS s = directory->Open(directory, &loadedFile, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

	if (s != EFI_SUCCESS)
	{
		return NULL;
	}

	// Returns the loaded file
	return loadedFile;
}

// Function to load the font file 
PSF1_FONT* LoadPSF1Font(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	Print(L"[INFO] Attempting to find font...\n\r");
	EFI_FILE* font = LoadFile(directory, path, imageHandle, systemTable);
	if (font == NULL) { Print(L"[FAIL] Failed to load font ;-;\n\r"); return NULL; }

	Print(L"[INFO] Successfully found font.\n\r");

	Print(L"[INFO] Verifying font header...\n\r");

	PSF1_HEADER* fontHeader;
	systemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);

	if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1)
	{ Print(L"[FAIL] Failed to verify font header ;-;\n\r"); return NULL; }

	Print(L"[INFO] Font header verified.\n\r");

	Print(L"[INFO] Loading font glyphs...\n\r");

	UINTN glyphBufferSize = fontHeader->charsize * 256;

	if(fontHeader->mode == 1)
	{ glyphBufferSize = fontHeader->charsize * 512; }

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		systemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF1_FONT* finishedFont;
	systemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);
	finishedFont->psf1_header = fontHeader;
	finishedFont->glyphbuffer = glyphBuffer;

	Print(L"[INFO] Successfully loaded glyphs.\n\r");

	return finishedFont;
}

// Function to initalize the graphics output protocol
FrameBuffer* initalizeGOP()
{
	EFI_GUID gopGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	Print(L"[INFO] Looking for graphics output protocol...\n\r");

	// Looks to see if the graphics protocol is supported
	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGUID, NULL, (void**)&gop);
	if (EFI_ERROR(status))
	{
		Print(L"[FAIL] Failed to find graphics output protocol ;-;\n\r");
		return NULL;
	}
	else
	{
		Print(L"[INFO] Found graphics output protocol.\n\r");
	}


	// Setups up a framebuffer
	frameBuffer.baseAddress = (void*)gop->Mode->FrameBufferBase;
	frameBuffer.bufferSize = gop->Mode->FrameBufferSize;
	frameBuffer.width = gop->Mode->Info->HorizontalResolution;
	frameBuffer.height = gop->Mode->Info->VerticalResolution;
	frameBuffer.pixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;

	// returns that framebuffer
	return &frameBuffer;
}



// Function to compare memory
int memcmp(const void* aptr, const void* bptr, size_t n)
{
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++)
	{
		if (a[i] < b[i]) return -1;
		else if(a[i] > b[i]) return 1;
	}
	return 0;
}

typedef struct
{
	FrameBuffer* frameBuffer;
	PSF1_FONT* font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	UINTN mMapSize;
	UINTN mMapDescSize;
} BootInfo;

// Our main boi
EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	// Initalize the image handle lib
	InitializeLib(ImageHandle, SystemTable);

	// KEKOS ascii logo
	Print(L"  _  __    _    ___  ___ \n\r | |/ /___| |__/ _ \\/ __|\n\r | ' </ -_) / / (_) \\__ \\\n\r |_|\\_\\___|_\\_\\\\___/|___/\n\r\n\r");

	Print(L"[INFO] Welcome to the KekOS UEFI bootloader!\n\r");
	Print(L"[INFO] Looking for the kernel...\n\r");
	// Looks for the kernel and loads it
	EFI_FILE* kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);
	if (kernel == NULL)
	{
		Print(L"[FAIL] Failed to find a kernel ;-;\n\r");
		Print(L"[FAIL] Exiting bootloader.\n\r");		
		return EFI_SUCCESS;
	}
	else 
	{
		Print(L"[INFO] Successfully found kernel.\n\r");
	}

	Print(L"[INFO] Verifying Kernel...\n\r");

	Elf64_Ehdr header;
	{
		UINTN fileInfoSize;
		EFI_FILE_INFO* fileInfo;
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, fileInfoSize, (void**)&fileInfo);
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileInfoSize, (void**)&fileInfo);
		UINTN size = sizeof(header);
		kernel->Read(kernel, &size, &header);
	}

	// Compare the kernel header to check if its valid or not
	if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	)
	{
		Print(L"[FAIL] Kernel failed verification ;-;\n\r");
		Print(L"[FAIL] Exiting bootloader.\n\r");
		return EFI_SUCCESS;
	}
	else 
	{
		Print(L"[INFO] Kernel successfully verified.\n\r");
	}

	Print(L"[INFO] Loading kernel...\n\r");

	// Load the kernel
	Elf64_Phdr* phdrs;
	{
		kernel->SetPosition(kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		kernel->Read(kernel, &size, phdrs);
	}

	for
	(
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
	)
	{
		switch (phdr->p_type)
		{
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				kernel->SetPosition(kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				kernel->Read(kernel, &size, (void*)segment);
			} break;
			default: break;
		}
	}

	Print(L"[INFO] Kernel successfully loaded.\n\r");

	// Pointer to the kernel start function
	void (*kernelStart)(BootInfo*) = ((__attribute((sysv_abi)) void (*)(BootInfo*) ) header.e_entry);


	PSF1_FONT* newFont = LoadPSF1Font(NULL, L"zap-light16.psf", ImageHandle, SystemTable);
	if (newFont == NULL) { Print(L"[FAIL] Failed to fully load font ;-;\n\r"); return EFI_SUCCESS; }
	else { Print(L"[INFO] Successfully loaded font.\n\r[INFO] Char size = %d\n\r", newFont->psf1_header->charsize); }


	// Initalize the graphics
	Print(L"[INFO] Initalizing graphics output protocol...\n\r");
	FrameBuffer* newBuffer = initalizeGOP();
	if (newBuffer == NULL) { Print(L"[FAIL] Exiting bootloader.\n\r"); return EFI_SUCCESS; }

	Print(L"[INFO] Framebuffer info:\n\r");
	Print(L"- Base:   %x\n\r- Size:   %x\n\r- Width:  %d\n\r- Height: %d\n\r- PPS:    %d\n\r", newBuffer->baseAddress, newBuffer->bufferSize, newBuffer->width, newBuffer->height, newBuffer->pixelsPerScanline);

	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	{
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}

	BootInfo bootInfo;
	bootInfo.frameBuffer = newBuffer;
	bootInfo.font = newFont;
	bootInfo.mMap = Map;
	bootInfo.mMapSize = MapSize;
	bootInfo.mMapDescSize = DescriptorSize;

	Print(L"[INFO] Quiting bootloader and starting kernel...\n\r");
	Print(L"[INFO] Goodbye o/\n\r");

	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	kernelStart(&bootInfo);

	Print(L"[FAIL] How are you here...\n\r[FAIL] You shouldn't be able to get here...\n\r[FAIL] What happened to the kernel?\n\r[FAIL] What did you do to my poor boy? ;-;\n\r[ERROR] (Somehow you fucked the kernel)\n\r[ERROR] (and you're back in the bootloader)\n\r[ERROR] (seriously how did you get here?)\n\r[ERROR CODE] HowDidYouGetHere\n\r");

	return EFI_SUCCESS; // Exit the UEFI application
}
