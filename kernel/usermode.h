#ifndef USERMODE_H
#define USERMODE_H

#include "../include/types.h"

// Switch to user mode (Ring 3)
void switch_to_user_mode();

// User mode test program entry point
void user_program();

#endif
