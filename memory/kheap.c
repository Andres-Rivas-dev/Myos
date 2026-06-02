#include "kheap.h"

extern uint32_t _kernel_end;

static uint8_t* heap_ptr = 0;
static uint8_t* heap_end = 0;

#define HEAP_SIZE (1024 * 1024) 

void kheap_init() {
    heap_ptr = (uint8_t*)&_kernel_end;
    heap_end = heap_ptr + HEAP_SIZE;
}

void* kmalloc(size_t size) {
    
    size = (size + 3) & ~3;
    if (heap_ptr + size > heap_end) return 0;  
    void* addr = heap_ptr;
    heap_ptr += size;
    return addr;
}


void kfree(void* ptr) {
    (void)ptr;  
}
