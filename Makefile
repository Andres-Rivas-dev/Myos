lib/kprintf.o: lib/kprintf.c
$(CC) -c lib/kprintf.c -o lib/kprintf.o $(CFLAGS) -I lib/ -I drivers/
CC      = i686-elf-gcc
AS      = nasm
CFLAGS  = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc

OBJS = boot/boot.o kernel/kernel.o kernel/gdt.o kernel/gdt_asm.o kernel/idt.o kernel/idt_asm.o drivers/vga.o drivers/keyboard.o drivers/ui.o drivers/mouse.o drivers/timer.o lib/string.o shell/shell.o shell/snake.o lib/kprintf.o

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
	rm -f $(OBJS) myos.bin myos.iso

shell/snake.o: shell/snake.c
	$(CC) -c shell/snake.c -o shell/snake.o $(CFLAGS) -I drivers/ -I lib/ -I shell/

lib/kprintf.o: lib/kprintf.c
$(CC) -c lib/kprintf.c -o lib/kprintf.o $(CFLAGS) -I lib/ -I drivers/
