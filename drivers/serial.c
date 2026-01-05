#include "../include/ports.h"
#include "../include/serial.h"

// Initialize Serial Port COM1
void serial_init() {
    port_byte_out(PORTS_COM1 + 1, 0x00);    // Disable all interrupts
    port_byte_out(PORTS_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    port_byte_out(PORTS_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    port_byte_out(PORTS_COM1 + 1, 0x00);    //                  (hi byte)
    port_byte_out(PORTS_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    port_byte_out(PORTS_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    port_byte_out(PORTS_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

// Check if Transmit Holding Register is empty
int is_transmit_empty() {
    return port_byte_in(PORTS_COM1 + 5) & 0x20;
}

void serial_write_char(char a) {
    while (is_transmit_empty() == 0);
    port_byte_out(PORTS_COM1, a);
}

void serial_print(char* str) {
    int i = 0;
    while (str[i] != 0) {
        serial_write_char(str[i]);
        i++;
    }
}
