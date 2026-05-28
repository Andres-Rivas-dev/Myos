#include "kprintf.h"
#include "string.h"
#include "../drivers/vga.h"
#include
static void print_int(int val, int base) {
char tmp[32];
int i = 0, neg = 0;
unsigned int uval;
if (base == 10 && val < 0) { neg = 1; uval = (unsigned int)(-val); }
else uval = (unsigned int)val;
if (uval == 0) { vga_putchar('0'); return; }
while (uval > 0) {
int r = uval % base;
tmp[i++] = r < 10 ? '0' + r : 'a' + (r - 10);
uval /= base;
}
if (neg) tmp[i++] = '-';
while (--i >= 0) vga_putchar(tmp[i]);
}
void kprintf(const char* fmt, ...) {
va_list args;
va_start(args, fmt);
int i;
for (i = 0; fmt[i]; i++) {
if (fmt[i] != '%') { vga_putchar(fmt[i]); continue; }
i++;
switch (fmt[i]) {
case 's': {
const char* s = va_arg(args, const char*);
vga_print(s ? s : "(null)");
break;
}
case 'd': print_int(va_arg(args, int), 10); break;
case 'x': print_int(va_arg(args, int), 16); break;
case 'c': vga_putchar((char)va_arg(args,int)); break;
case '%': vga_putchar('%'); break;
default: vga_putchar('%'); vga_putchar(fmt[i]); break;
}
}
va_end(args);
}