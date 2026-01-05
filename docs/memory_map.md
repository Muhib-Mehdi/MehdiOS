# Memory Map

This document describes the physical memory layout of the operating system during Phase 1.

## Physical Memory Layout

| Address Range       | Size   | Description                                 |
|---------------------|--------|---------------------------------------------|
| `0x00000 - 0x004FF` | 1 KB   | Real Mode IVT (Interrupt Vector Table)      |
| `0x00500 - 0x07BFF` | ~30 KB | Free usable memory                          |
| `0x07C00 - 0x07DFF` | 512 B  | **Boot Sector** (Loaded by BIOS)            |
| `0x07E00 - 0x0FFFF` | 32 KB  | Free usable memory (Bootloader Stack grows down from 0x8000) |
| `0x10000 - 0x9FFFF` | ~576 KB| Free usable memory (Kernel Stack grows down from 0x90000) |
| `0x01000 - ???????` | Varied | **Kernel Code & Data** (Loaded here)        |
| `0xA0000 - 0xBFFFF` | 128 KB | **Video Memory** (VGA Framebuffer)          |
| `0xC0000 - 0xFFFFF` | 256 KB | BIOS ROMs                                   |
| `0x100000 +`        | -      | Extended Memory (Free)                      |

## Key Addresses

- **0x7C00**: BIOS entry point.
- **0x1000**: Kernel entry point (Physical).
- **0xB8000**: VGA Text Mode Buffer (Color).
- **0x90000**: Top of Kernel Stack (initially).

## Stack Layout
- **Bootloader Stack base**: `0x8000`. Grows downwards towards `0x7E00`.
- **Protected Mode Stack base**: `0x90000`. Grows downwards towards kernel code options.
