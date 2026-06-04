#include "vga.h"
#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM    ((volatile uint16_t*)0xB8000)

/* Zona del shell: filas 2 a 23 */
#define SHELL_TOP    2
#define SHELL_BOTTOM 23

static int vga_row = SHELL_TOP;
static int vga_col = 0;
static uint8_t vga_attr = 0x0A;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
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
    vga_row  = SHELL_TOP;
    vga_col  = 0;
    vga_attr = 0x0A;
    vga_clear();
}

void vga_clear() {
    /* Solo limpia la zona del shell */
    for (int r = SHELL_TOP; r <= SHELL_BOTTOM; r++)
        for (int c = 0; c < VGA_WIDTH; c++)
            VGA_MEM[r * VGA_WIDTH + c] = make_entry(' ', vga_attr);
    vga_row = SHELL_TOP;
    vga_col = 0;
    update_cursor();
    vga_cursor_enable();
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_attr = (bg << 4) | (fg & 0x0F);
}

static void scroll() {
    /* Mueve filas SHELL_TOP+1 .. SHELL_BOTTOM hacia arriba */
    for (int r = SHELL_TOP; r < SHELL_BOTTOM; r++)
        for (int c = 0; c < VGA_WIDTH; c++)
            VGA_MEM[r * VGA_WIDTH + c] = VGA_MEM[(r+1) * VGA_WIDTH + c];
    /* Limpia la ultima fila del shell */
    for (int c = 0; c < VGA_WIDTH; c++)
        VGA_MEM[SHELL_BOTTOM * VGA_WIDTH + c] = make_entry(' ', vga_attr);
    vga_row = SHELL_BOTTOM;
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_col = 0;
        if (vga_row >= SHELL_BOTTOM) scroll();
        else vga_row++;
    } else if (c == '\r') {
        vga_col = 0;
    } else if (c == '\b') {
        if (vga_col > 0) vga_col--;
    } else {
        VGA_MEM[vga_row * VGA_WIDTH + vga_col] = make_entry(c, vga_attr);
        if (++vga_col >= VGA_WIDTH) {
            vga_col = 0;
            if (vga_row >= SHELL_BOTTOM) scroll();
            else vga_row++;
        }
    }
    update_cursor();
}

void vga_print(const char* str) {
    for (int i = 0; str[i]; i++)
        vga_putchar(str[i]);
}

int vga_get_col() { return vga_col; }


void vga_cursor_enable() {
    /* Activar cursor hardware: scanlines 14-15 (cursor al final del caracter) */
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}
