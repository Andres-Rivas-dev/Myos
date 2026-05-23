CC      = i686-elf-gcc
AS      = nasm
CFLAGS  = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc

OBJS = boot/boot.o kernel/kernel.o kernel/gdt.o kernel/gdt_asm.o

all: myos.iso

boot/boot.o: boot/boot.asm
	$(AS) $(ASFLAGS) boot/boot.asm -o boot/boot.o

kernel/kernel.o: kernel/kernel.c
	$(CC) -c kernel/kernel.c -o kernel/kernel.o $(CFLAGS) -I kernel/

kernel/gdt.o: kernel/gdt.c
	$(CC) -c kernel/gdt.c -o kernel/gdt.o $(CFLAGS) -I kernel/

kernel/gdt_asm.o: kernel/gdt_asm.asm
	$(AS) $(ASFLAGS) kernel/gdt_asm.asm -o kernel/gdt_asm.o

myos.bin: $(OBJS)
	$(CC) -T linker.ld -o myos.bin $(LDFLAGS) $(OBJS)

myos.iso: myos.bin
	cp myos.bin iso/boot/myos.bin
	grub-mkrescue -o myos.iso iso/

run: myos.iso
	qemu-system-i386 -cdrom myos.iso

clean:
	rm -f $(OBJS) myos.bin myos.iso
