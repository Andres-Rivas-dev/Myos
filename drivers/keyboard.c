#include "keyboard.h"
#include "vga.h"
#include <stdint.h>

static const char sc_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',
    0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,
    '7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

#define KBUF_SIZE 256
static char kbuf[KBUF_SIZE];
static int  kbuf_head = 0;
static int  kbuf_tail = 0;

void keyboard_irq_handler() {
    uint8_t sc = inb(0x60);
    if (sc & 0x80) return;
    char c = sc_map[sc & 0x7F];
    if (c) {
        int next = (kbuf_head + 1) % KBUF_SIZE;
        if (next != kbuf_tail) {
            kbuf[kbuf_head] = c;
            kbuf_head = next;
        }
    }
}

void keyboard_init() {
    kbuf_head = 0;
    kbuf_tail = 0;
}

char keyboard_getchar() {
    if (kbuf_head == kbuf_tail) return 0;
    char c = kbuf[kbuf_tail];
    kbuf_tail = (kbuf_tail + 1) % KBUF_SIZE;
    return c;
}
