#include "gdt.h"
#include "idt.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/ui.h"
#include "../shell/shell.h"

void kernel_main(void) {
    gdt_init();
    idt_init();
    vga_init();
    keyboard_init();

    __asm__ volatile ("sti");

    ui_boot_screen();
    ui_draw_desktop();
    shell_run();

    for(;;) {}
}
