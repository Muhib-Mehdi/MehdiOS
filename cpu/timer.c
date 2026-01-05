#include "timer.h"
#include "../cpu/isr.h"
#include "../include/ports.h"
#include "../include/util.h"
#include "../include/screen.h"
#include "../kernel/process.h"

uint32_t tick = 0;

static void timer_callback(registers_t regs) {
    tick++;
    
    // Call scheduler every tick for preemptive multitasking
    // Only if process manager is initialized (current process exists)
    if (get_current_process() != 0) {
        schedule();
    }
}

void init_timer(uint32_t freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);

    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
