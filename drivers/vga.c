#include "vga.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM    ((volatile unsigned short*)0xB8000)

static int vga_row = 0;
static int vga_col = 0;

static unsigned short make_entry(char c) {
    return (unsigned short)c | (0x0A << 8);
}

void vga_init() {
    vga_row = 0;
    vga_col = 0;
    vga_clear();
}

void vga_clear() {
    int i;
    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEM[i] = make_entry(' ');
    vga_row = 0;
    vga_col = 0;
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_row++;
        vga_col = 0;
    } else if (c == '\r') {
        vga_col = 0;
    } else if (c == '\b') {
        if (vga_col > 0) vga_col--;
    } else {
        VGA_MEM[vga_row * VGA_WIDTH + vga_col] = make_entry(c);
        if (++vga_col >= VGA_WIDTH) {
            vga_col = 0;
            vga_row++;
        }
    }
    if (vga_row >= VGA_HEIGHT) {
        int r, c;
        for (r = 1; r < VGA_HEIGHT; r++)
            for (c = 0; c < VGA_WIDTH; c++)
                VGA_MEM[(r-1)*VGA_WIDTH+c] = VGA_MEM[r*VGA_WIDTH+c];
        for (c = 0; c < VGA_WIDTH; c++)
            VGA_MEM[(VGA_HEIGHT-1)*VGA_WIDTH+c] = make_entry(' ');
        vga_row = VGA_HEIGHT - 1;
    }
}

void vga_print(const char* str) {
    int i;
    for (i = 0; str[i]; i++)
        vga_putchar(str[i]);
}
