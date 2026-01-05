#include "tss.h"
#include "../include/util.h"

tss_entry_t tss_entry;

// GDT entry structure (to update TSS descriptor)
extern void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void tss_init(uint32_t idx, uint32_t kernel_ss, uint32_t kernel_esp) {
    // Zero out the TSS
    memory_set((uint8_t*)&tss_entry, 0, sizeof(tss_entry_t));
    
    // Set the kernel stack segment and pointer
    tss_entry.ss0 = kernel_ss;
    tss_entry.esp0 = kernel_esp;
    
    // Set the CS, SS, DS, ES, FS, GS for kernel mode
    // Note: These aren't used for software task switching, but must be valid
    tss_entry.cs = 0x08;  // Kernel code segment
    tss_entry.ss = 0x10;  // Kernel data segment
    tss_entry.ds = 0x10;
    tss_entry.es = 0x10;
    tss_entry.fs = 0x10;
    tss_entry.gs = 0x10;
    
    // I/O map base - set to size of TSS (no I/O bitmap)
    tss_entry.iomap_base = sizeof(tss_entry_t);
}

void tss_set_kernel_stack(uint32_t esp) {
    tss_entry.esp0 = esp;
}
