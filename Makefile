lib/kprintf.o: lib/kprintf.c
$(CC) -c lib/kprintf.c -o lib/kprintf.o $(CFLAGS) -I lib/ -I drivers/
CC      = i686-elf-gcc
AS      = nasm
CFLAGS  = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc

all: myos.iso

boot/boot.o: boot/boot.asm
	$(AS) $(ASFLAGS) boot/boot.asm -o boot/boot.o

kernel/kernel.o: kernel/kernel.c
	$(CC) -c kernel/kernel.c -o kernel/kernel.o $(CFLAGS)

myos.bin: boot/boot.o kernel/kernel.o
	$(CC) -T linker.ld -o myos.bin $(LDFLAGS) boot/boot.o kernel/kernel.o

myos.iso: myos.bin
	cp myos.bin iso/boot/myos.bin
	grub-mkrescue -o myos.iso iso/

run: myos.iso
	qemu-system-i386 -cdrom myos.iso

clean:
	rm -f boot/boot.o kernel/kernel.o myos.bin myos.iso
