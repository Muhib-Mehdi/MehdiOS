#include "../include/util.h"

void memory_copy(char* source, char* dest, int n_bytes) {
    int i;
    for (i = 0; i < n_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t* dest, uint8_t val, uint32_t len) {
    uint8_t* temp = (uint8_t*)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void hex_to_ascii(int n, char str[]) {
    str[0] = '0';
    str[1] = 'x';
    int i = 2;
    int tmp;
    for (int j = 28; j >= 0; j -= 4) {
        tmp = (n >> j) & 0xF;
        if (tmp >= 0xA) {
            str[i] = tmp - 0xA + 'a';
        } else {
            str[i] = tmp + '0';
        }
        i++;
    }
    str[i] = 0;
}

int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

int strcmp_nocase(char s1[], char s2[]) {
    int i = 0;
    while (1) {
        char c1 = s1[i];
        char c2 = s2[i];
        
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        
        if (c1 != c2) return c1 - c2;
        if (c1 == '\0') return 0;
        i++;
    }
}
