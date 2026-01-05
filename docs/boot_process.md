# Boot Process

This document explains the step-by-step process from power-on to the C kernel execution.

## 1. BIOS Post
When the computer turns on, the BIOS (Basic Input/Output System) runs a Power-On Self-Test (POST). It looks for a bootable device (floppy, hard disk, USB).

## 2. Boot Sector Loading (Real Mode)
1. The BIOS finds our disk and reads the first **512 bytes** (Sector 0).
2. It verifies the generic signature `0xAA55` at the end of the sector.
3. It loads this code into memory address `0x7C00`.
4. The DL register contains the drive number we booted from.

## 3. Our Bootloader (`boot/boot_sect.asm`)
We are now executing in 16-bit Code Segment.
1. **Stack Setup**: We set `SS:SP` to `0x0000:0x8000`.
2. **Load Kernel**:
   - We use BIOS interrupt `0x13` to read sectors from disk logic.
   - We read 15 sectors (adjustable) starting from Sector 2 (Sector 1 is bootloader itself).
   - Destination address: `0x1000` (ES:BX = 0x0000:0x1000).
   - If this fails (Carry Flag set), we print an error and retry.
3. **Switch to Protected Mode**:
   - Disable interrupts (`cli`).
   - Load GDT Register (`lgdt`).
   - Copy bit 0 to CR0 register (PE bit).
   - Perform a "Far Jump" to `CODE_SEG:init_pm` to flush the CPU pipeline and switch to 32-bit mode.

## 4. Protected Mode (`boot/switch_pm.asm`)
We are now in 32-bit mode!
1. **Segment Registers**: We update DS, SS, ES, FS, GS to point to our Data Segment defined in GDT (0x10).
2. **Stack Update**: Move stack pointer to `0x90000` (top of free memory block).
3. **Jump to Kernel**: We jump to `0x1000`, where we loaded our kernel earlier.

## 5. Kernel Entry (`kernel/kernel_entry.asm`)
1. The execution lands at `0x1000`.
2. The assembly wrapper calls the external `main()` C function.
3. If `main()` returns, it enters an infinite halt loop.

## 6. C Kernel (`kernel/kernel.c`)
1. Clears screen (VGA 0xB8000).
2. Initializes Serial Port (COM1) for debugging.
3. Prints banner.
4. Enters main loop.
