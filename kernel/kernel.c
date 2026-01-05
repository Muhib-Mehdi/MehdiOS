#include "../include/kernel.h"
#include "../include/screen.h"
#include "../include/util.h"
#include "../include/serial.h"
#include "../cpu/isr.h"
#include "../cpu/gdt.h"
#include "../cpu/timer.h"
#include "../drivers/keyboard.h"
#include "shell.h"
#include "syscall.h"
#include "usermode.h"
#include "process.h"

// Debug Macros
#define ASSERT(b) ((b) ? (void)0 : panic(__FILE__, __LINE__, #b))

void panic(char* file, int line, char* desc) {
    // Print to screen
    kprint("\nPANIC: ");
    kprint(desc);
    kprint(" at ");
    kprint(file);
    kprint(":");
    char line_str[10];
    int_to_ascii(line, line_str);
    kprint(line_str);
    kprint("\n");
    
    // Print to serial
    serial_print("\nPANIC: ");
    serial_print(desc);
    serial_print(" at ");
    serial_print(file);
    serial_print(":");
    serial_print(line_str);
    serial_print("\n");

    // Halt
    while(1);
}

void main() {
    // 1. Initialize Screen
    clear_screen();
    kprint_at("Starting MehdiOS Kernel...", 0, 0);

    // 2. Initialize GDT with User Mode segments and TSS
    gdt_init();
    kprint("\n[OK] GDT initialized with User Mode segments\n");

    // 3. Initialize Interrupts
    isr_install();
    irq_install();
    kprint("[OK] Interrupts enabled\n");

    // 4. Initialize System Calls
    syscall_init();
    kprint("[OK] System calls registered (INT 0x80)\n");

    // 5. Initialize Process Manager
    process_init();

    // 6. Initialize Paging (Virtual Memory)
    // paging_init();  // Uncomment to enable paging

    // 7. Initialize Drivers
    init_keyboard();
    kprint("[OK] Keyboard driver ready\n");
    
    // 8. Initialize Timer (for preemptive multitasking)
    // init_timer(50);  // 50Hz - Uncomment to enable preemption
    // kprint("[OK] Timer enabled (50Hz)\n");
    
    // 9. Print Banner
    kprint("\n");
    kprint("   ##########################################\n");
    kprint("   #        Welcome to MehdiOS v0.5        #\n");
    kprint("   #     UserMode | Processes | Paging     #\n");
    kprint("   ##########################################\n");
    kprint("\nCommands: help, usermode, ps\n");
    serial_print("Kernel initialized.\n");
    
    // 10. Start Shell
    shell_init();

    // 11. Halt Loop
    while(1) {
        __asm__ __volatile__("hlt");
    }
}
