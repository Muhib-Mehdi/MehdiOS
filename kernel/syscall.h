#ifndef SYSCALL_H
#define SYSCALL_H

#include "../include/types.h"

// System call numbers
#define SYS_EXIT    0
#define SYS_WRITE   1
#define SYS_READ    2
#define SYS_OPEN    3
#define SYS_CLOSE   4

void syscall_init();

// System call implementations
int sys_exit(int code);
int sys_write(int fd, const char* buf, int len);
int sys_read(int fd, char* buf, int len);

#endif
