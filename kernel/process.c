#include "process.h"
#include "../include/screen.h"
#include "../include/util.h"
#include "mem.h"
#include "tss.h"

// Process table
static process_t processes[MAX_PROCESSES];
static uint32_t current_pid = 0;
static uint32_t next_pid = 1;

void process_init() {
    // Clear process table
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_UNUSED;
        processes[i].pid = 0;
    }
    
    // Create the kernel "process" (PID 0)
    processes[0].pid = 0;
    processes[0].state = PROCESS_RUNNING;
    memory_copy("kernel", processes[0].name, 7);
    current_pid = 0;
    
    kprint("[OK] Process manager initialized\n");
}

process_t* get_current_process() {
    return &processes[current_pid];
}

static int find_free_slot() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_UNUSED) {
            return i;
        }
    }
    return -1;
}

process_t* process_create(void (*entry_point)(), const char* name) {
    int slot = find_free_slot();
    if (slot < 0) {
        kprint("Error: No free process slots\n");
        return 0;
    }
    
    process_t* proc = &processes[slot];
    
    // Allocate stack
    uint32_t stack = kmalloc(PROCESS_STACK_SIZE, 1, 0);
    if (!stack) {
        kprint("Error: Could not allocate stack\n");
        return 0;
    }
    
    proc->pid = next_pid++;
    proc->state = PROCESS_READY;
    proc->stack_bottom = stack;
    
    // Setup initial stack frame for context switch
    // Stack grows downward, so start at top
    uint32_t* sp = (uint32_t*)(stack + PROCESS_STACK_SIZE);
    
    // Push fake return address (process_exit)
    // For now, just hang if process returns
    *(--sp) = 0;
    
    // Push entry point as EIP
    *(--sp) = (uint32_t)entry_point;
    
    // Push initial register values (will be popped by context_switch)
    *(--sp) = 0;  // EAX
    *(--sp) = 0;  // ECX
    *(--sp) = 0;  // EDX
    *(--sp) = 0;  // EBX
    *(--sp) = 0;  // ESP (ignored, we use the real one)
    *(--sp) = 0;  // EBP
    *(--sp) = 0;  // ESI
    *(--sp) = 0;  // EDI
    
    proc->esp = (uint32_t)sp;
    
    // Copy name
    int i = 0;
    while (name[i] && i < 31) {
        proc->name[i] = name[i];
        i++;
    }
    proc->name[i] = '\0';
    
    proc->time_slice = 10;  // 10 timer ticks per slice
    
    return proc;
}

void process_terminate(uint32_t pid) {
    if (pid == 0) {
        kprint("Cannot terminate kernel process\n");
        return;
    }
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROCESS_TERMINATED;
            kprint("Process terminated: ");
            kprint(processes[i].name);
            kprint("\n");
            return;
        }
    }
    kprint("Process not found\n");
}

void schedule() {
    // Simple round-robin scheduler
    // Find next ready process after current
    
    process_t* current = &processes[current_pid];
    
    int next = (current_pid + 1) % MAX_PROCESSES;
    int checked = 0;
    
    while (checked < MAX_PROCESSES) {
        if (processes[next].state == PROCESS_READY) {
            // Found a ready process - switch to it
            current->state = PROCESS_READY;
            processes[next].state = PROCESS_RUNNING;
            
            uint32_t old_pid = current_pid;
            current_pid = next;
            
            // Update TSS with new kernel stack
            tss_set_kernel_stack(processes[next].stack_bottom + PROCESS_STACK_SIZE);
            
            // Context switch
            context_switch(&processes[old_pid].esp, processes[next].esp);
            return;
        }
        next = (next + 1) % MAX_PROCESSES;
        checked++;
    }
    
    // No other ready processes, continue with current
}

void process_yield() {
    schedule();
}

void process_list() {
    kprint("PID  STATE      NAME\n");
    kprint("---  -----      ----\n");
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state != PROCESS_UNUSED) {
            char pid_str[8];
            int_to_ascii(processes[i].pid, pid_str);
            kprint(pid_str);
            kprint("    ");
            
            switch (processes[i].state) {
                case PROCESS_READY: kprint("READY  "); break;
                case PROCESS_RUNNING: kprint("RUNNING"); break;
                case PROCESS_BLOCKED: kprint("BLOCKED"); break;
                case PROCESS_TERMINATED: kprint("ZOMBIE "); break;
                default: kprint("???    "); break;
            }
            kprint("    ");
            kprint(processes[i].name);
            kprint("\n");
        }
    }
}
