#include "string.h"

size_t strlen(const char* s) {
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

char* strcpy(char* dst, const char* src) {
    char* r = dst;
    while ((*dst++ = *src++));
    return r;
}

void* memset(void* ptr, int val, size_t n) {
    unsigned char* p = (unsigned char*)ptr;
    while (n--) *p++ = (unsigned char)val;
    return ptr;
}
