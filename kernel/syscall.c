#include "syscall.h"
#include "../cpu/isr.h"
#include "../include/screen.h"
#include "../include/util.h"

static void syscall_callback(registers_t regs) {
    int ret = -1;
    
    switch (regs.eax) {
        case SYS_EXIT:
            ret = sys_exit(regs.ebx);
            break;
        case SYS_WRITE:
            ret = sys_write(regs.ebx, (const char*)regs.ecx, regs.edx);
            break;
        case SYS_READ:
            ret = sys_read(regs.ebx, (char*)regs.ecx, regs.edx);
            break;
        default:
            kprint("Unknown syscall: ");
            char s[4];
            int_to_ascii(regs.eax, s);
            kprint(s);
            kprint("\n");
            break;
    }
    
    // Return value would go in EAX but we can't modify regs directly
    // For now, syscalls are fire-and-forget
}

void syscall_init() {
    register_interrupt_handler(0x80, syscall_callback);
}

// ============ System Call Implementations ============

int sys_exit(int code) {
    kprint("Process exited with code: ");
    char s[10];
    int_to_ascii(code, s);
    kprint(s);
    kprint("\n");
    
    // For now, just hang (no process to kill yet)
    while(1);
    return 0;
}

int sys_write(int fd, const char* buf, int len) {
    // fd 1 = stdout (screen)
    if (fd == 1) {
        for (int i = 0; i < len; i++) {
            char str[2] = {buf[i], '\0'};
            kprint(str);
        }
        return len;
    }
    return -1;
}

int sys_read(int fd, char* buf, int len) {
    return -1;
}
