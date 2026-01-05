#ifndef TYPES_H
#define TYPES_H

// Freestanding environment, so we define fixed-width types manually
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef uint8_t bool;
#define true 1
#define false 0
#define NULL ((void*)0)

#endif
