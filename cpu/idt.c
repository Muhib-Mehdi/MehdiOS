#include "idt.h"
#include "../include/util.h"

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = (uint16_t)((handler) & 0xFFFF);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    
    // 0x8E = 1 (Present) 00 (Ring0) 0 (Reserved) 1110 (32-bit Interrupt Gate)
    idt[n].flags = 0x8E; 
    
    idt[n].high_offset = (uint16_t)(((handler) >> 16) & 0xFFFF);
}

// Same as above but with DPL=3 (allows user mode to call this interrupt)
void set_idt_gate_user(int n, uint32_t handler) {
    idt[n].low_offset = (uint16_t)((handler) & 0xFFFF);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    
    // 0xEE = 1 (Present) 11 (Ring3) 0 (Reserved) 1110 (32-bit Interrupt Gate)
    idt[n].flags = 0xEE; 
    
    idt[n].high_offset = (uint16_t)(((handler) >> 16) & 0xFFFF);
}

void set_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    /* Don't make the mistake of loading &idt -- try &idt_reg */
    __asm__ __volatile__("lidt (%0)" : : "r" (&idt_reg));
}
