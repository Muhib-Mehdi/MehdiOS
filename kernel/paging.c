#include "paging.h"
#include "mem.h"
#include "../include/screen.h"
#include "../include/util.h"
#include "../cpu/isr.h"

// Forward declaration
static void page_fault_isr(registers_t regs);

// Kernel page directory
static page_directory_t* kernel_directory = 0;

// External assembly function to load CR3
extern void load_page_directory(uint32_t* page_dir);
extern void enable_paging();

// Allocate a page-aligned block of memory
static uint32_t alloc_aligned(uint32_t size) {
    return kmalloc(size, 1, 0);  // 1 = aligned
}

void paging_init() {
    kprint("[  ] Initializing paging...\n");
    
    // Allocate space for kernel page directory
    kernel_directory = (page_directory_t*)alloc_aligned(sizeof(page_directory_t));
    memory_set((uint8_t*)kernel_directory, 0, sizeof(page_directory_t));
    
    // Identity map the first 4MB of memory (kernel space)
    // This means virtual address == physical address for kernel
    
    // Allocate page table for first 4MB
    page_table_t* first_table = (page_table_t*)alloc_aligned(sizeof(page_table_t));
    memory_set((uint8_t*)first_table, 0, sizeof(page_table_t));
    
    // Identity map: virtual 0x0 - 0x400000 -> physical 0x0 - 0x400000
    for (uint32_t i = 0; i < PAGES_PER_TABLE; i++) {
        uint32_t phys_addr = i * PAGE_SIZE;
        // Present + Writable + Supervisor (kernel only)
        first_table->entries[i] = phys_addr | PAGE_PRESENT | PAGE_WRITE;
    }
    
    // Set first directory entry to point to first page table
    kernel_directory->entries[0] = ((uint32_t)first_table) | PAGE_PRESENT | PAGE_WRITE;
    kernel_directory->tables[0] = first_table;
    
    // Also identity map 0xB8000 region (VGA)
    // VGA is at physical 0xB8000, which is in the first 4MB, so already mapped
    
    kernel_directory->physical_addr = (uint32_t)kernel_directory;
    
    // Register page fault handler
    register_interrupt_handler(14, page_fault_isr);
    
    // Load page directory into CR3 and enable paging
    switch_page_directory(kernel_directory);
    
    kprint("\r[OK] Paging enabled (4MB identity mapped)\n");
}

void switch_page_directory(page_directory_t* dir) {
    // Load CR3 with page directory physical address
    __asm__ __volatile__(
        "mov %0, %%cr3"
        :
        : "r" (dir->physical_addr)
    );
    
    // Enable paging in CR0
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;  // Set PG bit
    __asm__ __volatile__("mov %0, %%cr0" : : "r" (cr0));
}

page_directory_t* get_kernel_directory() {
    return kernel_directory;
}

// Page fault handler callback
static void page_fault_isr(registers_t regs) {
    // Get faulting address from CR2
    uint32_t faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r" (faulting_address));
    
    // Decode error code
    int present = !(regs.err_code & 0x1);   // Page not present
    int rw = regs.err_code & 0x2;           // Write operation
    int us = regs.err_code & 0x4;           // User mode
    int reserved = regs.err_code & 0x8;     // Reserved bits overwritten
    int id = regs.err_code & 0x10;          // Instruction fetch
    
    kprint("\n=== PAGE FAULT ===\n");
    kprint("Address: ");
    char s[16];
    hex_to_ascii(faulting_address, s);
    kprint(s);
    kprint("\n");
    
    if (present) kprint("  - Page not present\n");
    if (rw) kprint("  - Write operation\n");
    if (us) kprint("  - User mode\n");
    if (reserved) kprint("  - Reserved bits set\n");
    if (id) kprint("  - Instruction fetch\n");
    
    kprint("System halted.\n");
    while(1);
}

void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
    // Get page directory and table indices
    uint32_t pd_index = virtual_addr >> 22;           // Top 10 bits
    uint32_t pt_index = (virtual_addr >> 12) & 0x3FF; // Next 10 bits
    
    // Check if page table exists
    if (!(kernel_directory->entries[pd_index] & PAGE_PRESENT)) {
        // Allocate new page table
        page_table_t* new_table = (page_table_t*)alloc_aligned(sizeof(page_table_t));
        memory_set((uint8_t*)new_table, 0, sizeof(page_table_t));
        
        kernel_directory->entries[pd_index] = ((uint32_t)new_table) | PAGE_PRESENT | PAGE_WRITE | flags;
        kernel_directory->tables[pd_index] = new_table;
    }
    
    // Get the page table
    page_table_t* table = kernel_directory->tables[pd_index];
    
    // Map the page
    table->entries[pt_index] = (physical_addr & 0xFFFFF000) | PAGE_PRESENT | flags;
}

uint32_t alloc_page(uint32_t virtual_addr, uint32_t flags) {
    // Allocate a physical page
    uint32_t physical_addr = alloc_aligned(PAGE_SIZE);
    
    // Map virtual to physical
    map_page(virtual_addr, physical_addr, flags);
    
    return physical_addr;
}
