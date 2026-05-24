#include "gdt.h"
#include "idt.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"

void kernel_main(void) {
    gdt_init();
    idt_init();
    vga_init();
    keyboard_init();

    __asm__ volatile ("sti");

    vga_print("MyOS v0.1 - UFG 2025\n");
    vga_print("Integrantes: Stefano, Bryan, Kenneth, Andres\n");
    vga_print("\nEscribe algo:\n");

    while (1) {
        char c = keyboard_getchar();
        if (c) vga_putchar(c);
    }
}


