#include "vga.h"
#include "keyboard.h"

void vga_init()   {}
void vga_clear()  {}
void vga_print(const char* s) { (void)s; }
void vga_putchar(char c) { (void)c; }
void vga_set_color(vga_color_t fg, vga_color_t bg) { (void)fg; (void)bg; }
char keyboard_getchar() { return 0; }
