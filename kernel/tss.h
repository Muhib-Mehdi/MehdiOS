#ifndef TSS_H
#define TSS_H

#include "../include/types.h"

/*
 * Task State Segment (TSS)
 * Used by the CPU for hardware task switching and
 * storing the kernel stack pointer for Ring 0.
 */
typedef struct {
    uint32_t prev_tss;   // Previous TSS link (unused)
    uint32_t esp0;       // Stack pointer for Ring 0 (kernel)
    uint32_t ss0;        // Stack segment for Ring 0
    uint32_t esp1;       // Stack pointer for Ring 1 (unused)
    uint32_t ss1;
    uint32_t esp2;       // Stack pointer for Ring 2 (unused)
    uint32_t ss2;
    uint32_t cr3;        // Page directory base
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

void tss_init(uint32_t idx, uint32_t kernel_ss, uint32_t kernel_esp);
void tss_set_kernel_stack(uint32_t esp);
void tss_flush();

#endif
