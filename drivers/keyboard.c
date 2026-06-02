#include "keyboard.h"
#include <stdint.h>

static const char sc_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',
    0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* Scancodes extendidos (después de 0xE0) */
#define SC_UP    0x48
#define SC_DOWN  0x50
#define SC_LEFT  0x4B
#define SC_RIGHT 0x4D

static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

#define KBUF_SIZE 256
static char kbuf[KBUF_SIZE];
static int  kbuf_head = 0;
static int  kbuf_tail = 0;
static int  extended  = 0;

static void kbuf_push(char c) {
    int next = (kbuf_head + 1) % KBUF_SIZE;
    if (next != kbuf_tail) {
        kbuf[kbuf_head] = c;
        kbuf_head = next;
    }
}

void keyboard_irq_handler() {
    uint8_t sc = inb(0x60);

    if (sc == 0xE0) { extended = 1; return; }

    if (sc & 0x80) { extended = 0; return; }

    if (extended) {
        extended = 0;
        if      (sc == SC_UP)    kbuf_push(KEY_UP);
        else if (sc == SC_DOWN)  kbuf_push(KEY_DOWN);
        else if (sc == SC_LEFT)  kbuf_push(KEY_LEFT);
        else if (sc == SC_RIGHT) kbuf_push(KEY_RIGHT);
        return;
    }

    char c = sc_map[sc & 0x7F];
    if (c) kbuf_push(c);
}

void keyboard_init() {
    kbuf_head = 0;
    kbuf_tail = 0;
    extended  = 0;
}

char keyboard_getchar() {
    if (kbuf_head == kbuf_tail) return 0;
    char c = kbuf[kbuf_tail];
    kbuf_tail = (kbuf_tail + 1) % KBUF_SIZE;
    return c;
}
