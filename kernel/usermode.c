#include "usermode.h"
#include "syscall.h"

/*
 * User Program - Runs in Ring 3 (User Mode)
 * This program can ONLY interact with the kernel via system calls.
 * Direct access to hardware or kernel memory will cause a fault.
 */

// Inline assembly to make a system call
static inline int syscall(int num, int arg1, int arg2, int arg3) {
    int ret;
    __asm__ __volatile__ (
        "int $0x80"
        : "=a" (ret)
        : "a" (num), "b" (arg1), "c" (arg2), "d" (arg3)
    );
    return ret;
}

// Wrapper functions for user programs
static int write(int fd, const char* buf, int len) {
    return syscall(SYS_WRITE, fd, (int)buf, len);
}

static void exit(int code) {
    syscall(SYS_EXIT, code, 0, 0);
    while(1);  // Should never reach here
}

// Calculate string length (user-space version)
static int ustrlen(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

// The actual user program entry point
void user_program() {
    // DEBUG: Write directly to VGA to prove we got here
    // This bypasses syscalls - if we see "U" at top of screen, IRET worked
    volatile char* vga = (char*)0xB8000;
    vga[0] = 'U';  // Write 'U' to top-left
    vga[1] = 0x4F; // White on red - very visible
    
    // Make syscall to write
    const char* msg = "Hello from User Mode (Ring 3)!\n";
    write(1, msg, ustrlen(msg));
    
    const char* msg2 = "System calls are working!\n";
    write(1, msg2, ustrlen(msg2));
    
    // Exit cleanly
    exit(0);
}
