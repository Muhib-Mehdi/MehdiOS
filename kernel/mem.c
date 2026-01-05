#include "mem.h"
#include "../include/screen.h"

/* 
 * Heap Start: 0x10000 (64KB) - Just experimental.
 * The kernel stack is at 0x90000.
 * The kernel code is at 0x1000.
 * Let's place the heap safely above the kernel code but below the stack.
 * Or even better, above 1MB (0x100000) if we had paging enabled.
 * For now, simple placement address.
 */
uint32_t free_mem_addr = 0x10000;

void memory_init() {
    // Just a placeholder for now
    free_mem_addr = 0x10000; 
}

/* 
 * Implementation is just a pointer increment.
 * It's not standard malloc (can't free), but enough for static things.
 */
uint32_t kmalloc(uint32_t size, int align, uint32_t *phys_addr) {
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    
    if (phys_addr) *phys_addr = free_mem_addr;
    
    uint32_t ret = free_mem_addr;
    free_mem_addr += size;
    return ret;
}
