#ifndef VGA_H
#define VGA_H
#include <stdint.h>
#include <stddef.h>

void vga_init();
void vga_clear();
void vga_putchar(char c);
void vga_print(const char* str);
#endif
