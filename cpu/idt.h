#ifndef IDT_H
#define IDT_H

#include "../include/types.h"

// Segment selectors
#define KERNEL_CS 0x08

// Interrupt Descriptor definitions
typedef struct {
    uint16_t low_offset;   // Lower 16 bits of handler function address
    uint16_t sel;          // Kernel segment selector
    uint8_t always0;       // Always 0
    uint8_t flags;         // Type and attributes (0x8E = Interrupt Gate)
    uint16_t high_offset;  // Higher 16 bits of handler function address
} __attribute__((packed)) idt_gate_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
extern idt_gate_t idt[IDT_ENTRIES];
extern idt_register_t idt_reg;

// Functions
void set_idt_gate(int n, uint32_t handler);
void set_idt_gate_user(int n, uint32_t handler);  // DPL=3 for user mode access
void set_idt();

#endif
