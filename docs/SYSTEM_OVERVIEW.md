# MehdiOS: Complete System Documentation

## Table of Contents
1. [Overview](#overview)
2. [Boot Process](#boot-process)
3. [Memory Architecture](#memory-architecture)
4. [Interrupt System](#interrupt-system)
5. [Drivers](#drivers)
6. [File System](#file-system)
7. [Shell](#shell)
8. [Source File Reference](#source-file-reference)

---

## Overview

**MehdiOS** is a 32-bit operating system kernel written from scratch in C and x86 Assembly. It demonstrates:
- Custom bootloader (no GRUB)
- Protected Mode operation
- Hardware interrupt handling
- Device drivers (Keyboard, Screen, Disk)
- Interactive command shell
- File system reading capability

**Technical Specifications:**
| Property | Value |
|----------|-------|
| Architecture | x86 (32-bit) |
| Boot Mode | Real Mode → Protected Mode |
| Memory Model | Flat Memory (4GB addressable) |
| Kernel Location | 0x1000 (Physical) |
| Stack Location | 0x90000 |
| VGA Mode | Text Mode (80x25) |

---

## Boot Process

### Stage 1: BIOS Initialization
When the computer powers on, the BIOS:
1. Performs POST (Power-On Self-Test)
2. Searches for bootable media
3. Loads the first 512 bytes (MBR) from disk to memory address `0x7C00`
4. Jumps to `0x7C00` to execute the bootloader

### Stage 2: Bootloader (`boot/boot_sect.asm`)
Our custom bootloader performs these steps:

```
0x7C00: Boot Sector Loaded
    │
    ├─► Save Boot Drive (DL register)
    ├─► Setup Stack at 0x8000
    ├─► Print "Started in 16-bit Real Mode"
    │
    ├─► Load Kernel from Disk
    │   └─► Uses BIOS INT 0x13 (disk_load.asm)
    │   └─► Loads 50 sectors (25KB) to 0x1000
    │
    ├─► Switch to Protected Mode
    │   ├─► Disable Interrupts (CLI)
    │   ├─► Load GDT (Global Descriptor Table)
    │   ├─► Set CR0 PE bit
    │   └─► Far Jump to 32-bit code
    │
    └─► Jump to Kernel at 0x1000
```

### Stage 3: GDT (Global Descriptor Table)
The GDT defines memory segments for Protected Mode:

| Segment | Base | Limit | Access | Purpose |
|---------|------|-------|--------|---------|
| Null | 0x0 | 0x0 | 0x00 | Required by CPU |
| Code | 0x0 | 0xFFFFFFFF | 0x9A | Executable, Readable |
| Data | 0x0 | 0xFFFFFFFF | 0x92 | Writable |

We use a **Flat Memory Model** where Code and Data segments both span the entire 4GB address space.

### Stage 4: Kernel Entry (`kernel/kernel_entry.asm`)
```asm
[bits 32]
[extern main]
_start:
    call main      ; Call C kernel
    jmp $          ; Hang if main returns
```

---

## Memory Architecture

### Physical Memory Map
```
0x00000000 ┌─────────────────────────┐
           │ Real Mode IVT           │ (1KB)
0x00000400 ├─────────────────────────┤
           │ BIOS Data Area          │
0x00000500 ├─────────────────────────┤
           │ Free Memory             │
0x00001000 ├─────────────────────────┤ ◄── KERNEL_OFFSET
           │ ████ KERNEL CODE ████   │
           │ .text section           │
           │ .rodata (strings)       │
           │ .data (initialized)     │
           │ .bss (uninitialized)    │
0x00010000 ├─────────────────────────┤ ◄── HEAP START
           │ Dynamic Memory (Heap)   │
           │ kmalloc allocations     │
0x00090000 ├─────────────────────────┤ ◄── STACK
           │ Kernel Stack (grows ↓)  │
0x0009FFFF ├─────────────────────────┤
           │ Extended BIOS Data      │
0x000A0000 ├─────────────────────────┤
           │ VGA Video Memory        │ (128KB)
0x000B8000 │ Text Mode Buffer        │
0x000C0000 ├─────────────────────────┤
           │ BIOS ROM                │
0x00100000 └─────────────────────────┘ (1MB)
```

### Kernel Memory Manager (`kernel/mem.c`)
Simple "placement" allocator:
```c
uint32_t free_mem_addr = 0x10000;

uint32_t kmalloc(uint32_t size, int align, uint32_t *phys_addr) {
    // Page-align if requested
    if (align && (free_mem_addr & 0xFFF)) {
        free_mem_addr = (free_mem_addr & 0xFFFFF000) + 0x1000;
    }
    uint32_t ret = free_mem_addr;
    free_mem_addr += size;
    return ret;
}
```

**Limitations:**
- No `kfree()` - memory cannot be reclaimed
- No paging - physical addresses only
- No memory protection

---

## Interrupt System

### Interrupt Descriptor Table (IDT)
The IDT maps interrupt numbers to handler functions:

| Vector | Type | Handler | Description |
|--------|------|---------|-------------|
| 0 | Exception | isr0 | Divide by Zero |
| 1 | Exception | isr1 | Debug |
| 6 | Exception | isr6 | Invalid Opcode |
| 13 | Exception | isr13 | General Protection Fault |
| 14 | Exception | isr14 | Page Fault |
| 32 | IRQ0 | irq0 | Timer (PIT) |
| 33 | IRQ1 | irq1 | Keyboard |
| 46 | IRQ14 | irq14 | Primary ATA |

### PIC Remapping
The 8259 PIC (Programmable Interrupt Controller) is remapped to avoid conflicts:
- Master PIC: IRQs 0-7 → Vectors 32-39
- Slave PIC: IRQs 8-15 → Vectors 40-47

```c
// Initialization Command Words (ICW)
port_byte_out(0x20, 0x11);  // ICW1: Initialize Master
port_byte_out(0xA0, 0x11);  // ICW1: Initialize Slave
port_byte_out(0x21, 0x20);  // ICW2: Master offset = 32
port_byte_out(0xA1, 0x28);  // ICW2: Slave offset = 40
```

### Interrupt Flow
```
Hardware Event (e.g., Key Press)
    │
    ▼
PIC receives IRQ1
    │
    ▼
CPU receives INT 33
    │
    ▼
IDT lookup → irq1 (cpu/interrupt.asm)
    │
    ▼
Save registers (pusha)
    │
    ▼
Call irq_handler(registers_t r) in C
    │
    ▼
Dispatch to keyboard_callback()
    │
    ▼
Send EOI to PIC
    │
    ▼
Restore registers (popa)
    │
    ▼
Return from interrupt (iret)
```

---

## Drivers

### VGA Text Mode Driver (`drivers/screen.c`)

**Memory-mapped I/O at 0xB8000:**
Each character cell = 2 bytes:
- Byte 0: ASCII character
- Byte 1: Attribute (foreground/background color)

```
Attribute Byte:
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│  7  │  6  │  5  │  4  │  3  │  2  │  1  │  0  │
├─────┴─────┴─────┴─────┼─────┴─────┴─────┴─────┤
│    Background (4)     │    Foreground (4)     │
└───────────────────────┴───────────────────────┘
```

**Color Constants:**
| Value | Color |
|-------|-------|
| 0x0 | Black |
| 0x1 | Blue |
| 0x2 | Green |
| 0x7 | Light Grey |
| 0xF | White |

**Functions:**
- `kprint(char *message)` - Print string at cursor
- `kprint_at(char *message, int col, int row)` - Print at position
- `kprint_backspace()` - Delete last character
- `clear_screen()` - Fill screen with spaces

**Cursor Control:**
Uses VGA Controller ports `0x3D4` (index) and `0x3D5` (data):
```c
void set_cursor_offset(int offset) {
    offset /= 2;  // Character cells, not bytes
    port_byte_out(0x3D4, 14);  // High byte register
    port_byte_out(0x3D5, (offset >> 8) & 0xFF);
    port_byte_out(0x3D4, 15);  // Low byte register
    port_byte_out(0x3D5, offset & 0xFF);
}
```

### Keyboard Driver (`drivers/keyboard.c`)

**PS/2 Keyboard Interface:**
- Data port: `0x60`
- Status port: `0x64`

**Scancode Set 1 (Make Codes):**
| Key | Scancode |
|-----|----------|
| 1 | 0x02 |
| Q | 0x10 |
| A | 0x1E |
| Enter | 0x1C |
| Backspace | 0x0E |
| Left Shift | 0x2A |
| Caps Lock | 0x3A |

**Features:**
- Lowercase/Uppercase via Shift or Caps Lock
- Backspace deletes characters
- Enter submits command to shell
- Full ASCII printable characters

### ATA/IDE Disk Driver (`drivers/ata.c`)

**PIO Mode 28-bit LBA:**
| Port | Read | Write |
|------|------|-------|
| 0x1F0 | Data | Data |
| 0x1F1 | Error | Features |
| 0x1F2 | Sector Count | Sector Count |
| 0x1F3 | LBA Low | LBA Low |
| 0x1F4 | LBA Mid | LBA Mid |
| 0x1F5 | LBA High | LBA High |
| 0x1F6 | Drive/Head | Drive/Head |
| 0x1F7 | Status | Command |

**Read Sector Algorithm:**
```c
void ata_read_sectors(uint32_t lba, uint8_t count, uint8_t* buffer) {
    // 1. Select drive and LBA mode
    port_byte_out(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    
    // 2. Send sector count and LBA
    port_byte_out(0x1F2, count);
    port_byte_out(0x1F3, lba & 0xFF);
    port_byte_out(0x1F4, (lba >> 8) & 0xFF);
    port_byte_out(0x1F5, (lba >> 16) & 0xFF);
    
    // 3. Send READ command
    port_byte_out(0x1F7, 0x20);
    
    // 4. Wait for data and read
    for (int i = 0; i < 256; i++) {
        buffer[i*2] = port_word_in(0x1F0);
    }
}
```

### Serial Port Driver (`drivers/serial.c`)

**COM1 at 0x3F8:**
Used for kernel debugging output to host terminal.

```c
void serial_init() {
    port_byte_out(0x3F8 + 1, 0x00);  // Disable interrupts
    port_byte_out(0x3F8 + 3, 0x80);  // Enable DLAB
    port_byte_out(0x3F8 + 0, 0x03);  // Baud rate divisor (38400)
    port_byte_out(0x3F8 + 3, 0x03);  // 8N1
}
```

---

## File System

### FAT12 Structure (`fs/fat12.c`)

**Disk Layout:**
```
┌────────────────────┐ Sector 0
│ Boot Sector (BPB)  │
├────────────────────┤ Sector 1
│ FAT Table 1        │
├────────────────────┤
│ FAT Table 2 (copy) │
├────────────────────┤
│ Root Directory     │
├────────────────────┤
│ Data Area          │
└────────────────────┘
```

**Boot Parameter Block (BPB):**
```c
typedef struct {
    uint8_t  boot_jump[3];
    uint8_t  oem_name[8];
    uint16_t bytes_per_sector;      // Usually 512
    uint8_t  sectors_per_cluster;   // Usually 1
    uint16_t reserved_sector_count; // Usually 1
    uint8_t  table_count;           // Usually 2
    uint16_t root_entry_count;      // Usually 224
    // ... more fields
} fat_boot_sector_t;
```

**Directory Entry (32 bytes):**
```c
typedef struct {
    uint8_t  name[8];        // 8.3 filename
    uint8_t  ext[3];         // Extension
    uint8_t  attributes;     // File attributes
    uint16_t first_cluster;  // Starting cluster
    uint32_t size;           // File size in bytes
} fat_directory_entry_t;
```

**Limitations:**
- Read-only (no write support)
- Requires FAT12-formatted disk image
- 8.3 filename format only

---

## Shell

### Command Interpreter (`kernel/shell.c`)

**Available Commands:**
| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show available commands | `help` |
| `clear` | Clear screen | `clear` |
| `echo <text>` | Print text back | `echo Hello World` |
| `mem` | Test memory allocation | `mem` |
| `readsec` | Read boot sector | `readsec` |
| `ls` | List root directory | `ls` |

**Command Processing Flow:**
```
User types "help" + Enter
    │
    ▼
Keyboard driver buffers input
    │
    ▼
Enter key triggers user_input(buffer)
    │
    ▼
strcmp() matches "help"
    │
    ▼
Print help text
    │
    ▼
Print new prompt "> "
```

---

## Source File Reference

### Boot (`boot/`)
| File | Lines | Purpose |
|------|-------|---------|
| `boot_sect.asm` | 79 | Main bootloader |
| `disk_load.asm` | 66 | BIOS disk read |
| `gdt.asm` | 105 | GDT definition |
| `switch_pm.asm` | 58 | Protected mode switch |
| `print_real_mode.asm` | 70 | Real mode print utils |

### CPU (`cpu/`)
| File | Lines | Purpose |
|------|-------|---------|
| `idt.h/c` | ~30 | IDT structures |
| `isr.h/c` | ~130 | Interrupt handlers |
| `interrupt.asm` | 280 | Low-level ISR stubs |
| `timer.h/c` | 30 | PIT timer driver |

### Drivers (`drivers/`)
| File | Lines | Purpose |
|------|-------|---------|
| `screen.h/c` | 120 | VGA text output |
| `keyboard.h/c` | 85 | PS/2 keyboard |
| `ports.h/c` | 25 | I/O port access |
| `serial.h/c` | 35 | COM1 debugging |
| `ata.h/c` | 50 | IDE disk driver |

### Kernel (`kernel/`)
| File | Lines | Purpose |
|------|-------|---------|
| `kernel.c` | 70 | Main entry point |
| `shell.h/c` | 65 | Command interpreter |
| `mem.h/c` | 35 | Memory allocator |
| `util.h/c` | 85 | String/memory utils |
| `kernel_entry.asm` | 14 | ASM entry stub |
| `linker.ld` | 37 | Linker script |

### File System (`fs/`)
| File | Lines | Purpose |
|------|-------|---------|
| `fat12.h` | 50 | FAT12 structures |
| `fat12.c` | 90 | FAT12 parser |

---

## Build System

### Makefile Targets
```bash
make          # Build os-image.bin
make run      # Build and run in QEMU
make debug    # Run with GDB debugging
make clean    # Remove build artifacts
make hexdump  # View binary contents
```

### Compilation Pipeline
```
Source Files
    │
    ├─► boot_sect.asm ──► nasm -f bin ──► boot_sect.bin
    │
    ├─► *.c ──► gcc -m32 -ffreestanding ──► *.o
    │
    └─► *.asm ──► nasm -f elf ──► *.o
                      │
                      ▼
              ld -T linker.ld ──► kernel.bin
                      │
                      ▼
              cat boot_sect.bin kernel.bin ──► os-image.bin
```

---

## Future Development

Planned features for future phases:
1. **Paging** - Virtual memory support
2. **Multitasking** - Process scheduler
3. **User Mode** - Ring 3 execution
4. **System Calls** - INT 0x80 interface
5. **File Writing** - FAT12 write support
6. **Networking** - NE2000 driver

---

*MehdiOS v0.2 - Built for MIT Portfolio*
