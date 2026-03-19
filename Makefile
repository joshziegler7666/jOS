LANG="C"
TARGET=boot.efi
COMMON = /usr/lib/crt0-efi-x86_64.o
LDFLAGS = -T /usr/lib/elf_x86_64_efi.lds -Bsymbolic -shared -nostdlib -znocombreloc
CFLAGS=-I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol \
       -DGNU_EFI_USE_MS_ABI -fPIC -fshort-wchar -ffreestanding \
       -fno-stack-protector -mno-red-zone -m64 \
       -Wall -Werror

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
		-j .rela -j .reloc -S --target=efi-app-x86_64 $^ $@

%.so: %.o
	$(LD) $(LDFLAGS) -o $@ $(COMMON) $^ /usr/lib/libefi.a /usr/lib/libgnuefi.a \
		$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)


efi: kernel.bin $(TARGET)
	mkdir -p ./boot
	cp $(TARGET) ./boot/$(TARGET)
	cp kernel.bin ./boot/kernel.bin

kernel.bin: kernel.c
	gcc -ffreestanding -fno-stack-protector -nostdlib -nostdinc \
		-static -no-pie \
		-m64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
		-Werror -Ttext 0x100000 -e kernel_main \
		-o kernel.elf kernel.c
	objcopy -O binary kernel.elf kernel.bin
		
clean:
	rm -f $(TARGET) *.so *.o kernel.bin
	rm -rf ./boot

run:
	qemu-system-x86_64 -bios OVMF.fd -hda fat:rw:./boot/ -net none -monitor stdio

all:
	make clean
	make
	make run
