CC      = i686-elf-gcc
AS      = nasm
CFLAGS  = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc

OBJS = boot/boot.o kernel/kernel.o kernel/gdt.o kernel/gdt_asm.o kernel/idt.o kernel/idt_asm.o drivers/vga.o drivers/keyboard.o drivers/ui.o lib/string.o shell/shell.o

all: myos.iso

boot/boot.o: boot/boot.asm
	$(AS) $(ASFLAGS) boot/boot.asm -o boot/boot.o

kernel/kernel.o: kernel/kernel.c
	$(CC) -c kernel/kernel.c -o kernel/kernel.o $(CFLAGS) -I kernel/ -I drivers/ -I shell/

kernel/gdt.o: kernel/gdt.c
	$(CC) -c kernel/gdt.c -o kernel/gdt.o $(CFLAGS) -I kernel/

kernel/gdt_asm.o: kernel/gdt_asm.asm
	$(AS) $(ASFLAGS) kernel/gdt_asm.asm -o kernel/gdt_asm.o

kernel/idt.o: kernel/idt.c
	$(CC) -c kernel/idt.c -o kernel/idt.o $(CFLAGS) -I kernel/ -I drivers/

kernel/idt_asm.o: kernel/idt_asm.asm
	$(AS) $(ASFLAGS) kernel/idt_asm.asm -o kernel/idt_asm.o

drivers/vga.o: drivers/vga.c
	$(CC) -c drivers/vga.c -o drivers/vga.o $(CFLAGS) -I drivers/

drivers/keyboard.o: drivers/keyboard.c
	$(CC) -c drivers/keyboard.c -o drivers/keyboard.o $(CFLAGS) -I drivers/

drivers/ui.o: drivers/ui.c
	$(CC) -c drivers/ui.c -o drivers/ui.o $(CFLAGS) -I drivers/ -I lib/

lib/string.o: lib/string.c
	$(CC) -c lib/string.c -o lib/string.o $(CFLAGS) -I lib/

shell/shell.o: shell/shell.c
	$(CC) -c shell/shell.c -o shell/shell.o $(CFLAGS) -I drivers/ -I lib/ -I shell/

myos.bin: $(OBJS)
	$(CC) -T linker.ld -o myos.bin $(LDFLAGS) $(OBJS)

myos.iso: myos.bin
	cp myos.bin iso/boot/myos.bin
	grub-mkrescue -o myos.iso iso/

run: myos.iso
	qemu-system-i386 -cdrom myos.iso

clean:
	rm -f $(OBJS) myos.bin myos.iso
