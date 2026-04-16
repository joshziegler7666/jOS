/*
 * the structs i needed from efi.h
*/
#ifndef EFI_HEADER_H
#define EFI_HEADER_H

typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;
typedef void               VOID;
typedef UINT16             CHAR16;
typedef UINTN              EFI_STATUS;
typedef VOID               *EFI_HANDLE;
typedef UINT64             EFI_PHYSICAL_ADDRESS;

#define EFIAPI __attribute__((ms_abi))
#define NULL   ((VOID*)0)
#define EFI_SUCCESS 0
#define EFI_FILE_MODE_READ 0x0000000000000001

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8  Data4[8];
} EFI_GUID;

typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
} EFI_MEMORY_TYPE;

typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
} EFI_ALLOCATE_TYPE;

typedef struct {
    UINT32               Type;
    UINT32               Pad;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_PHYSICAL_ADDRESS VirtualStart;
    UINT64               NumberOfPages;
    UINT64               Attribute;
} EFI_MEMORY_DESCRIPTOR;

// Forward declare
typedef struct _EFI_FILE EFI_FILE;

typedef struct _EFI_FILE_IO_INTERFACE {
    UINT64 Revision;
    EFI_STATUS (EFIAPI *OpenVolume)(struct _EFI_FILE_IO_INTERFACE *This, EFI_FILE **Root);
} EFI_FILE_IO_INTERFACE;

struct _EFI_FILE {
    UINT64 Revision;
    EFI_STATUS (EFIAPI *Open)  (EFI_FILE *This, EFI_FILE **NewHandle, CHAR16 *FileName, UINT64 OpenMode, UINT64 Attributes);
    EFI_STATUS (EFIAPI *Close) (EFI_FILE *This);
    EFI_STATUS (EFIAPI *Delete)(EFI_FILE *This);
    EFI_STATUS (EFIAPI *Read)  (EFI_FILE *This, UINTN *BufferSize, VOID *Buffer);
    EFI_STATUS (EFIAPI *Write) (EFI_FILE *This, UINTN *BufferSize, VOID *Buffer);
};

#define SIMPLE_FILE_SYSTEM_PROTOCOL (EFI_GUID){0x964e5b22,0x6459,0x11d2,{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

typedef struct {
    UINT32      Revision;
    EFI_HANDLE  ParentHandle;
    VOID        *SystemTable;
    EFI_HANDLE  DeviceHandle;
    VOID        *FilePath;
    VOID        *Reserved;
    UINT32      LoadOptionsSize;
    VOID        *LoadOptions;
    VOID        *ImageBase;
    UINT64      ImageSize;
    UINT32      ImageCodeType;
    UINT32      ImageDataType;
    VOID        *Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

#define EFI_LOADED_IMAGE_PROTOCOL_GUID  (EFI_GUID){ 0x5b1b31a1, 0x9562, 0x11d2, { 0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b }}

typedef struct {
    UINT32            RedMask;
    UINT32            GreenMask;
    UINT32            BlueMask;
    UINT32            ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
    UINT32            Version;
    UINT32            HorizontalResolution;
    UINT32            VerticalResolution;
    UINT32            PixelFormat;
    EFI_PIXEL_BITMASK PixelInformation;
    UINT32            PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32                               MaxMode;
    UINT32                               Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN                                SizeOfInfo;
    EFI_PHYSICAL_ADDRESS                 FrameBufferBase;
    UINTN                                FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    VOID                              *QueryMode;
    VOID                              *SetMode;
    VOID                              *Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    (EFI_GUID){0x9042a9de,0x23dc,0x4a38,{0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_INTERFACE {
    VOID       *Reset;
    EFI_STATUS (EFIAPI *OutputString)(struct _EFI_SIMPLE_TEXT_OUTPUT_INTERFACE *This, CHAR16 *String);
} EFI_SIMPLE_TEXT_OUTPUT_INTERFACE;

//dont need most
typedef struct {
    EFI_TABLE_HEADER Hdr;
    VOID *RaiseTPL;
    VOID *RestoreTPL;
    EFI_STATUS (EFIAPI *AllocatePages)(EFI_ALLOCATE_TYPE, EFI_MEMORY_TYPE, UINTN, EFI_PHYSICAL_ADDRESS*);
    VOID *FreePages;
    EFI_STATUS (EFIAPI *GetMemoryMap)(UINTN*, EFI_MEMORY_DESCRIPTOR*, UINTN*, UINTN*, UINT32*);
    EFI_STATUS (EFIAPI *AllocatePool)(EFI_MEMORY_TYPE, UINTN, VOID**);
    VOID *FreePool;
    VOID *CreateEvent;
    VOID *SetTimer;
    VOID *WaitForEvent;
    VOID *SignalEvent;
    VOID *CloseEvent;
    VOID *CheckEvent;
    VOID *InstallProtocolInterface;
    VOID *ReinstallProtocolInterface;
    VOID *UninstallProtocolInterface;
    EFI_STATUS (EFIAPI *HandleProtocol)(EFI_HANDLE Handle, EFI_GUID *Protocol, VOID **Interface);
    VOID *Reserved;
    VOID *RegisterProtocolNotify;
    VOID *LocateHandle;
    VOID *LocateDevicePath;
    VOID *InstallConfigurationTable;
    VOID *LoadImage;
    VOID *StartImage;
    VOID *Exit;
    VOID *UnloadImage;
    EFI_STATUS (EFIAPI *ExitBootServices)(EFI_HANDLE, UINTN);
    VOID *GetNextMonotonicCount;
    VOID *Stall;
    VOID *SetWatchdogTimer;
    VOID *ConnectController;
    VOID *DisconnectController;
    VOID *OpenProtocol;
    VOID *CloseProtocol;
    VOID *OpenProtocolInformation;
    VOID *ProtocolsPerHandle;
    VOID *LocateHandleBuffer;
    EFI_STATUS (EFIAPI *LocateProtocol)(EFI_GUID*, VOID*, VOID**);
    VOID *InstallMultipleProtocolInterfaces;
    VOID *UninstallMultipleProtocolInterfaces;
    VOID *CalculateCrc32;
    VOID *CopyMem;
    VOID *SetMem;
    VOID *CreateEventEx;
} EFI_BOOT_SERVICES;

typedef struct {
    EFI_TABLE_HEADER                  Hdr;
    CHAR16                            *FirmwareVendor;
    UINT32                            FirmwareRevision;
    EFI_HANDLE                        ConsoleInHandle;
    VOID                              *ConIn;
    EFI_HANDLE                        ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_INTERFACE  *ConOut;
    EFI_HANDLE                        StandardErrorHandle;
    VOID                              *StdErr;
    VOID                              *RuntimeServices;
    EFI_BOOT_SERVICES                 *BootServices;
} EFI_SYSTEM_TABLE;


typedef struct {
    UINT32                            *fb;
    UINT32                            HorizontalResolution;
    UINT32                            VerticalResolution;
} kernelParameters;


#endif
