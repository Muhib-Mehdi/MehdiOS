#ifndef UTIL_H
#define UTIL_H

#include "types.h"

void memory_copy(char* source, char* dest, int n_bytes);
void memory_set(uint8_t* dest, uint8_t val, uint32_t len);
int strlen(char s[]);
int strcmp(char s1[], char s2[]);
int strcmp_nocase(char s1[], char s2[]);
void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char s[]);

#endif
