#ifndef PAGING_H
#define PAGING_H

#include "../include/types.h"

// Page flags
#define PAGE_PRESENT    0x1
#define PAGE_WRITE      0x2
#define PAGE_USER       0x4
#define PAGE_ACCESSED   0x20
#define PAGE_DIRTY      0x40

// Page sizes
#define PAGE_SIZE       4096
#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIR  1024

// Page structures
typedef uint32_t page_table_entry_t;
typedef uint32_t page_directory_entry_t;

typedef struct {
    page_table_entry_t entries[PAGES_PER_TABLE];
} page_table_t;

typedef struct {
    page_directory_entry_t entries[TABLES_PER_DIR];
    // Physical addresses of page tables (virtual addresses can differ)
    page_table_t* tables[TABLES_PER_DIR];
    uint32_t physical_addr;
} page_directory_t;

// Functions
void paging_init();
void switch_page_directory(page_directory_t* dir);
page_directory_t* get_kernel_directory();

// Map a virtual address to a physical address
void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

// Allocate and map a page
uint32_t alloc_page(uint32_t virtual_addr, uint32_t flags);

// Page fault handler
void page_fault_handler(uint32_t error_code, uint32_t faulting_address);

#endif
