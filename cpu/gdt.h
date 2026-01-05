#ifndef GDT_H
#define GDT_H

#include "../include/types.h"

// Segment selectors
#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_CS   0x18
#define USER_DS   0x20
#define TSS_SEL   0x28

// Ring 3 selectors need RPL bits set
#define USER_CS_RPL (USER_CS | 0x3)
#define USER_DS_RPL (USER_DS | 0x3)

void gdt_init();
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

#endif
