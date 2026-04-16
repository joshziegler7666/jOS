/*  
 *  
 *  this is not verry easy to read.
**/

#include "efiHeader.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"loading th thing\r\n");

    //get the GOP for the kernel
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    SystemTable->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, ((VOID*)0), (void**)&gop);
    
    EFI_FILE_IO_INTERFACE *fs;
    SystemTable->BootServices->LocateProtocol(&SIMPLE_FILE_SYSTEM_PROTOCOL, ((VOID*)0), (void**)&fs);


    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"open volume\r\n");
    //open ./boot 
    EFI_FILE *bootVolume;
    fs->OpenVolume(fs, &bootVolume);

    //open kernel.bin
    EFI_FILE *kernel_file;
    bootVolume->Open(bootVolume, &kernel_file, L"kernel.bin", EFI_FILE_MODE_READ, 0);

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"allocate\r\n");
    
    //allocate 1 mb at 0x100000 and read into it
    UINTN kernelLocation = 0x100000;
    UINTN allocationSize =  0x400000; //1mb / 4096 is  256 pages 
    UINTN kernelSize = 0x300000; //3mb read size
    EFI_STATUS status = SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, allocationSize / 4096, (EFI_PHYSICAL_ADDRESS[]){kernelLocation});
    if (status != 0) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"AllocatePages failed\r\n");
        for(;;);
    }
    kernel_file->Read(kernel_file, &kernelSize, (void*)kernelLocation);
    kernel_file->Close(kernel_file);

    //exit boot services
    UINTN map_size = 0, map_key, desc_size;
    UINT32 desc_ver;
    EFI_MEMORY_DESCRIPTOR *map = NULL;//((VOID*)0);
    SystemTable->BootServices->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_ver);
    map_size += 4 * desc_size;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, map_size, (void**)&map);
    SystemTable->BootServices->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_ver);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"acctualy exit\r\n");
    SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

    volatile int *breadcrumb = (volatile int*)0x7000;
    *breadcrumb = 0xDEADBEEF;

    //kernel starts at 0x100000, so put the function pointer there. 
    kernelParameters kp;
    kp.fb = (UINT32*)gop->Mode->FrameBufferBase;
    kp.HorizontalResolution = gop->Mode->Info->HorizontalResolution;
    kp.VerticalResolution = gop->Mode->Info->VerticalResolution;
    void (*kernel_main)(kernelParameters*) = (void (*)(kernelParameters*))kernelLocation;
    kernel_main(&kp);

    //infinate loop
    for(;;);
    return EFI_SUCCESS;
}
