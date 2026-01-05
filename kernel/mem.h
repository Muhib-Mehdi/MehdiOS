#ifndef MEM_H
#define MEM_H

#include "../include/types.h"

/*
 * Simple Memory Manager
 * We will define a heap starting at 0x100000 (1MB) since kernel is at 0x1000.
 */

void memory_init();
uint32_t kmalloc(uint32_t size, int align, uint32_t *phys_addr);

#endif
