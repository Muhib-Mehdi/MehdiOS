# MehdiOS: A MIT Portfolio Operating System

A 32-bit protected mode operating system kernel written from scratch in C and Assembly.
This project demonstrates low-level systems programming, memory management, and hardware interaction.

## Features Application
### Phase 1: Foundation
- **Custom Bootloader**: 512-byte MBR (Assembly) that switches to 32-bit Protected Mode.
- **GDT**: Global Descriptor Table for flat memory model.
- **VGA Driver**: Text mode (80x25) with scrolling and cursor support.
- **Serial Debugging**: Logs kernel events to COM1 for headless debugging.

### Phase 2: Core Features
- **Interrupts (IDT)**: Full Interrupt Descriptor Table handling CPU exceptions (Divide-by-zero, etc.).
- **PIC Remapping**: Programmable Interrupt Controller remapped to avoid conflicts.
- **Keyboard Driver**: Interrupt-driven (IRQ1) PS/2 keyboard support.
- **Shell**: Interactive command-line interface with `help`, `echo`, `clear`.
- **Memory Manager**: Basic `kmalloc` placement heap allocator.

### Phase 3: Advanced
- **ATA Driver**: Protected Mode PIO driver to read hard disk sectors.
- **File System**: FAT12 parsing logic (implemented) and `ls`/`cat` commands.

## Setup Requirements
**Windows Users**: Use **WSL2 (Ubuntu)**.
See [Windows Setup Guide](docs/WINDOWS_SETUP.md).

```bash
# Install Tools (Ubuntu/WSL)
sudo apt update
sudo apt install build-essential nasm qemu-system-x86 gcc-multilib
```

## Building and Running
```bash
# Build
make

# Run (QEMU)
make run

# Debug
make debug
```

## Directory Structure
- `boot/`: Assembly bootloader & GDT.
- `kernel/`: Core C kernel, Shell, Memory Manager.
- `cpu/`: Interrupts (IDT, ISR) and Timer.
- `drivers/`: Hardware drivers (Screen, Keyboard, ATA, Serial).
- `fs/`: File System logic (FAT12).
- `docs/`: Architecture documentation & Demo Script.
