#ifndef PROCESS_H
#define PROCESS_H

#include "../include/types.h"

#define MAX_PROCESSES 16
#define PROCESS_STACK_SIZE 4096

// Process states
typedef enum {
    PROCESS_UNUSED = 0,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

// Process Control Block (PCB)
typedef struct {
    uint32_t pid;               // Process ID
    process_state_t state;      // Current state
    
    // CPU registers (saved during context switch)
    uint32_t esp;               // Stack pointer
    uint32_t ebp;               // Base pointer
    uint32_t eip;               // Instruction pointer
    uint32_t eflags;            // Flags
    
    // Segment registers
    uint32_t cs;
    uint32_t ds;
    
    // Memory
    uint32_t stack_bottom;      // Bottom of allocated stack
    uint32_t page_directory;    // For future paging support
    
    // Metadata
    char name[32];
    uint32_t priority;
    uint32_t time_slice;        // Remaining time slice
} process_t;

// Process management functions
void process_init();
process_t* process_create(void (*entry_point)(), const char* name);
void process_terminate(uint32_t pid);
void process_yield();
void schedule();
process_t* get_current_process();
void process_list();

// Context switch (defined in switch.asm)
extern void context_switch(uint32_t* old_esp, uint32_t new_esp);

#endif
