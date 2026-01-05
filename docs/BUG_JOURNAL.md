# Bug Journal - MehdiOS Development

## Overview
This document chronicles significant bugs encountered during development, their root causes, and how they were resolved.

---

## Bug #1: Garbage Output During Boot

**Symptom:** Random characters displayed instead of kernel messages after boot.

**Root Cause:** Bootloader was only loading 15 sectors, but kernel grew beyond that size.

**Debugging Process:**
1. Added serial debugging output
2. Noticed kernel entry was reached but VGA writes showed garbage
3. Checked kernel binary size: exceeded loaded sectors
4. Increased sector count in bootloader

**Fix:** Changed `mov dh, 15` to `mov dh, 50` in boot_sect.asm

**Lesson:** Always verify binary size vs loaded sectors during development.

---

## Bug #2: Keyboard Not Responding

**Symptom:** Keys typed but no characters appeared on screen.

**Root Cause:** Keyboard interrupt handler registered but interrupts not enabled.

**Debugging Process:**
1. Verified IRQ1 handler was registered
2. Checked PIC was properly remapped
3. Found `sti` instruction was missing after IRQ setup

**Fix:** Added `__asm__("sti");` in irq_install()

**Lesson:** Always enable interrupts after setting up handlers.

---

## Bug #3: Shell Commands Not Working

**Symptom:** Typing "help" did nothing, unknown command appeared.

**Root Cause:** Keyboard was outputting uppercase letters, but commands were lowercase.

**Debugging Process:**
1. Added debug print of raw input buffer
2. Saw "HELP" instead of "help"
3. Checked keyboard scancode table - was using uppercase array

**Fix:** Created separate lowercase/uppercase scancode arrays, added Shift key detection

**Lesson:** Test exact string values, not just "it looks right".

---

## Bug #4: INT 0x80 Causing Exception

**Symptom:** "received interrupt: -128" displayed when calling syscall.

**Root Cause:** Two issues:
1. `push byte 128` sign-extends to -128
2. ISR handler was treating all interrupts as exceptions

**Debugging Process:**
1. Noticed interrupt number was negative
2. Checked assembly - byte push of 128 causes sign extension
3. ISR handler didn't check for registered handlers

**Fix:** 
1. Changed to `push dword 128`
2. Modified isr_handler to check handlers[] first

**Lesson:** Be careful with signed/unsigned in assembly.

---

## Bug #5: User Mode Transition Hang

**Symptom:** System hung after "Switching to User Mode..."

**Root Cause:** Multiple issues with IRET stack frame setup.

**Debugging Process:**
1. Added VGA debug character at start of user_program
2. Character didn't appear → problem before reaching user code
3. Checked IRET stack order: SS, ESP, EFLAGS, CS, EIP
4. Found segment selectors weren't including RPL bits

**Fix:** Changed segment selectors to include RPL=3 (0x1B, 0x23)

**Lesson:** IRET stack frame order and segment selector format are critical.

---

## Bug #6: Caps Lock Inverted

**Symptom:** Caps Lock ON produced lowercase, OFF produced uppercase.

**Root Cause:** Initial caps_lock_on state didn't match host system state.

**Debugging Process:**
1. User reported behavior was backwards
2. Traced through keyboard callback logic
3. Realized QEMU passes through host Caps Lock state

**Fix:** Changed XOR logic to OR, adjusted initial state

**Lesson:** Virtualized input can have host-dependent behavior.

---

## Debugging Tools Used

| Tool | Purpose |
|------|---------|
| Serial Output | Print debug messages to terminal |
| GDB | Step through kernel code |
| QEMU Monitor | Inspect registers, memory |
| Hexdump | Verify binary contents |
| VGA Direct Write | Confirm code execution point |
