#include "keyboard.h"
#include "io.h"

static const char sc_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',
    0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,
    '7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0
};

char keyboard_getchar() {
    uint8_t sc;
    while (1) {
        /* esperar a que el buffer tenga dato (bit 0 del puerto 0x64) */
        while (!(inb(0x64) & 0x01));
        sc = inb(0x60);
        if (sc & 0x80) continue;       /* tecla suelta, ignorar */
        if (sc_map[sc]) return sc_map[sc];
    }
}
