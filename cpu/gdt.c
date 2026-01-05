#include "gdt.h"
#include "../kernel/tss.h"
#include "../include/util.h"

// GDT entry structure
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

// 6 entries: null, kernel code, kernel data, user code, user data, tss
gdt_entry_t gdt_entries[6];
gdt_ptr_t gdt_ptr;
extern tss_entry_t tss_entry;

extern void gdt_flush(uint32_t);
extern void tss_flush();

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt_entries[num].access = access;
}

void gdt_init() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;
    
    // Null segment
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // Kernel Code segment (0x08): base=0, limit=4GB, code, ring 0
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Kernel Data segment (0x10): base=0, limit=4GB, data, ring 0
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // User Code segment (0x18): base=0, limit=4GB, code, ring 3
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    // User Data segment (0x20): base=0, limit=4GB, data, ring 3  
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    
    // TSS segment (0x28)
    uint32_t tss_base = (uint32_t)&tss_entry;
    uint32_t tss_limit = sizeof(tss_entry_t);
    gdt_set_gate(5, tss_base, tss_limit, 0x89, 0x00);
    
    // Initialize TSS
    tss_init(5, KERNEL_DS, 0x90000);  // Kernel stack at 0x90000
    
    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
}
