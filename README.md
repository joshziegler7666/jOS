# jOS

A 64-bit x86 operating system written from scratch in C, with no external libraries. jOS boots through a custom UEFI bootloader, loads its own kernel into memory, and runs an interactive shell drawn pixel by pixel to the framebuffer.

![jOS screenshot](screenshots/screenshot1.png)

## Features

- **Custom UEFI bootloader** (`boot.c`)
  - Locates the Graphics Output Protocol (GOP) to get the framebuffer
  - Reads `kernel.bin` from the boot volume into memory at a fixed address (`0x100000`)
  - Retrieves the UEFI memory map and calls `ExitBootServices`
  - Passes framebuffer info to the kernel and jumps to its entry point
- **Freestanding kernel** (`kernel.c`)
  - Framebuffer graphics with a custom 8x8 bitmap font and adjustable text scaling
  - PS/2 keyboard driver that reads scan codes directly from I/O ports `0x60`/`0x64` using inline assembly in c
  - Shift key handling, backspace, and line editing
  - Interactive shell with command parsing and error messages
  No standard library. String handling, number parsing, and hex conversion are all implemented by hand.

## Shell Commands

 Command | Description 

 `help` | Lists available commands 
 `backgroundColor 0xRRGGBB` | Changes the background color 
 `consoleColor 0xRRGGBB` | Changes the console text color 
 `scale N` | Changes text scale (N / 10, so `15` = 1.5x) 

## Building and Running

### Requirements

- Linux
- `gcc`, `binutils` (`ld`, `objcopy`), `make`
- [GNU-EFI](https://sourceforge.net/projects/gnu-efi/) (headers in `/usr/include/efi`, libraries in `/usr/lib`)
- QEMU (`qemu-system-x86_64`)
- `OVMF.fd` (UEFI firmware for QEMU) in the project directory

On Debian:

```bashfortnite freddy fazbear
sudo apt install build-essential gnu-efi qemu-system-x86 ovmf
```

### Build and run

```bash
make        # builds boot.efi and kernel.bin into ./boot
make run    # boots it in QEMU
make all    # delete it build it agian and run
```

### Debugging with GDB

```bash
make runDebug     # starts QEMU paused with a GDB server on port 1234
```

In another terminal:

```bash
gdb kernel.elf
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
```

## How It Works

1. UEFI firmware loads `boot.efi` from the FAT boot volume.
2. The bootloader finds the framebuffer through GOP and opens `kernel.bin`.
3. It allocates pages at `0x100000` and reads the kernel binary there.
4. It fetches the memory map and exits boot services, so firmware no longer controls the machine.
5. It calls `kernel_main` at `0x100000`, passing the framebuffer address and resolution.
6. The kernel takes over the screen and polls the keyboard controller for input.

## Roadmap

Physical memory manager using the UEFI memory map
Scrolling console
Interrupt handling for applications
