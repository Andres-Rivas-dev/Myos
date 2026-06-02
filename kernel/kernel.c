#include "gdt.h"
#include "idt.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../shell/shell.h"

void kernel_main(void) {
    gdt_init();
    idt_init();
    vga_init();
    keyboard_init();

    __asm__ volatile ("sti");

    vga_print("MyOS v0.1 - UFG 2025\n");
    vga_print("GDT OK | IDT OK | Teclado OK\n");
    vga_print("================================\n");

    shell_run();

    for(;;) {}
}


