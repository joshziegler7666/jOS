#include "efiHeader.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"loading th thing\r\n");

    //get GOP
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    SystemTable->BootServices->LocateProtocol(&gop_guid, ((VOID*)0), (void**)&gop);
    
    //load kernel
    EFI_FILE_IO_INTERFACE *fs;
    EFI_GUID fs_guid = SIMPLE_FILE_SYSTEM_PROTOCOL;
    SystemTable->BootServices->LocateProtocol(&fs_guid, ((VOID*)0), (void**)&fs);

    EFI_FILE *root;
    fs->OpenVolume(fs, &root);

    //open kernel.bin
    EFI_FILE *kernel_file;
    root->Open(root, &kernel_file, L"kernel.bin", EFI_FILE_MODE_READ, 0);

    // Allocate memory at 0x100000 and read into it
    UINTN kernel_size = 0x100000; // 1MB, adjust as needed
    SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData,
        kernel_size / 4096, (EFI_PHYSICAL_ADDRESS[]){0x100000});

    kernel_file->Read(kernel_file, &kernel_size, (void*)0x100000);
    kernel_file->Close(kernel_file);

    // Exit boot services
    UINTN map_size = 0, map_key, desc_size;
    UINT32 desc_ver;
    EFI_MEMORY_DESCRIPTOR *map = ((VOID*)0);
    SystemTable->BootServices->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_ver);
    map_size += 4 * desc_size;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, map_size, (void**)&map);
    SystemTable->BootServices->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_ver);
    SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

    // kernel starts at 0x100000
    kernelParameters kp;
    kp.fb = (UINT32*)gop->Mode->FrameBufferBase;
    kp.HorizontalResolution = gop->Mode->Info->HorizontalResolution;
    kp.VerticalResolution = gop->Mode->Info->VerticalResolution;
    void (*kernel_main)(kernelParameters*) = (void (*)(kernelParameters*))0x100000;
    kernel_main(&kp);

    for(;;);
    return EFI_SUCCESS;
}
