#ifndef SERIAL_H
#define SERIAL_H

// COM1 Port Address
#define PORTS_COM1 0x3f8

void serial_init();
void serial_check();
void serial_print(char* str);

#endif
