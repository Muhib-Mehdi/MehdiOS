#include "keyboard.h"
#include "../include/ports.h"
#include "../cpu/isr.h"
#include "../include/screen.h"
#include "../include/util.h"
#include "../kernel/shell.h" // For user_input

#define SC_MAX 57
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define L_SHIFT 0x2A
#define R_SHIFT 0x36
#define L_SHIFT_REL 0xAA
#define R_SHIFT_REL 0xB6
#define CAPS_LOCK 0x3A

static char key_buffer[256];
static int shift_pressed = 0;
static int caps_lock_on = 0;

static void keyboard_callback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);

    /* Handle Shift Press/Release */
    if (scancode == L_SHIFT || scancode == R_SHIFT) {
        shift_pressed = 1;
        return;
    } else if (scancode == L_SHIFT_REL || scancode == R_SHIFT_REL) {
        shift_pressed = 0;
        return;
    } else if (scancode == CAPS_LOCK) {
        caps_lock_on = !caps_lock_on; /* Toggle */
        return;
    }
    
    char sc_ascii_lower[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
        '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y', 
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v', 
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

    char sc_ascii_upper[] = { '?', '?', '!', '@', '#', '$', '%', '^',     
        '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '{', '}', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ':', '"', '~', '?', '|', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', '<', '>', '?', '?', '?', '?', ' '};

    if (scancode > SC_MAX) return;
    
    if (scancode == BACKSPACE) {
        int len = strlen(key_buffer);
        if (len > 0) {
            kprint_backspace();
            key_buffer[len - 1] = '\0';
        }
    } else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer); /* Call the shell */
        key_buffer[0] = '\0'; /* Clear buffer */
    } else {
        char letter;
        int use_upper = shift_pressed || caps_lock_on; /* Either one triggers uppercase */
        if (use_upper) {
            letter = sc_ascii_upper[(int)scancode];
        } else {
            letter = sc_ascii_lower[(int)scancode];
        }
        
        /* Remember to add null byte for printable */
        char str[2] = {letter, '\0'};
        kprint(str);
        
        /* Append to buffer */
        int len = strlen(key_buffer);
        if (len < 255) {
            key_buffer[len] = letter;
            key_buffer[len+1] = '\0';
        }
    }
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}
