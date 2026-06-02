#include "vga.h"
#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM    ((volatile uint16_t*)0xB8000)

static int vga_row = 0;
static int vga_col = 0;
static uint8_t vga_attr = 0x0A;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

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
    vga_attr = 0x0A;
    vga_clear();
}

void vga_clear() {
    int i;
    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEM[i] = make_entry(' ', vga_attr);
    vga_row = 0;
    vga_col = 0;
    update_cursor();
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_attr = (bg << 4) | (fg & 0x0F);
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
        VGA_MEM[vga_row * VGA_WIDTH + vga_col] = make_entry(c, vga_attr);
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
            VGA_MEM[(VGA_HEIGHT-1)*VGA_WIDTH+c] = make_entry(' ', vga_attr);
        vga_row = VGA_HEIGHT - 1;
    }
    update_cursor();
}

void vga_print(const char* str) {
    int i;
    for (i = 0; str[i]; i++)
        vga_putchar(str[i]);
}
