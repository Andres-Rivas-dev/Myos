#include "vga.h"
#include "io.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM    ((uint16_t*)0xB8000)

static size_t vga_row, vga_col;
static uint8_t vga_attr;

static inline uint16_t make_entry(char c, uint8_t attr) {
    return (uint16_t)c | ((uint16_t)attr << 8);
}

static void update_cursor() {
    uint16_t pos = vga_row * VGA_WIDTH + vga_col;
    outb(0x3D4, 14); outb(0x3D5, pos >> 8);
    outb(0x3D4, 15); outb(0x3D5, pos & 0xFF);
}

void vga_init() {
    vga_row = 0;
    vga_col = 0;
    vga_attr = (VGA_BLACK << 4) | VGA_LIGHT_GREY;
    vga_clear();
}

void vga_clear() {
    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++)
        VGA_MEM[i] = make_entry(' ', vga_attr);
    vga_row = vga_col = 0;
    update_cursor();
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    vga_attr = (bg << 4) | (fg & 0x0F);
}

void vga_putchar(char c) {
    if (c == '\n') { vga_row++; vga_col = 0; }
    else if (c == '\r') { vga_col = 0; }
    else if (c == '\b') { if (vga_col) vga_col--; }
    else {
        VGA_MEM[vga_row * VGA_WIDTH + vga_col] = make_entry(c, vga_attr);
        if (++vga_col >= VGA_WIDTH) { vga_col = 0; vga_row++; }
    }
    if (vga_row >= VGA_HEIGHT) {
        for (size_t r = 1; r < VGA_HEIGHT; r++)
            for (size_t c2 = 0; c2 < VGA_WIDTH; c2++)
                VGA_MEM[(r-1)*VGA_WIDTH+c2] = VGA_MEM[r*VGA_WIDTH+c2];
        for (size_t c2 = 0; c2 < VGA_WIDTH; c2++)
            VGA_MEM[(VGA_HEIGHT-1)*VGA_WIDTH+c2] = make_entry(' ', vga_attr);
        vga_row = VGA_HEIGHT - 1;
    }
    update_cursor();
}

void vga_print(const char* str) {
    for (size_t i = 0; str[i]; i++)
        vga_putchar(str[i]);
}
