# Design Decisions in MehdiOS

## Overview
This document explains key architectural decisions made during development.

---

## 1. Bootloader Choice: Custom vs GRUB

**Decision:** Custom bootloader

**Why:**
- Demonstrates deep understanding of x86 boot process
- Full control over memory layout
- Educational value higher than using GRUB
- Smaller footprint (~512 bytes)

**Tradeoff:**
- More complex to implement
- No multiboot support
- Manual protected mode transition required

---

## 2. Kernel Load Address: 0x1000

**Decision:** Load kernel at physical address 0x1000

**Why:**
- Below 1MB limit for BIOS access
- Above IVT (0x0000-0x03FF) and BDA (0x0400-0x04FF)
- Leaves room for bootloader at 0x7C00
- Simple identity mapping when paging enabled

**Alternatives considered:**
- 0x100000 (1MB): Requires unreal mode or protected mode disk access
- 0x7E00: Too close to bootloader, limits kernel size

---

## 3. File System: FAT12

**Decision:** Implement FAT12 read-only support

**Why:**
- Simplest FAT variant (12-bit cluster entries)
- Standard for floppy disks (1.44MB)
- Well-documented specification
- QEMU easily creates FAT12 images

**Tradeoffs:**
- 4078 max clusters (limited size)
- No long filename support (8.3 only)
- Read-only implementation

---

## 4. Scheduling: Round-Robin

**Decision:** Simple round-robin scheduler

**Why:**
- Fair time distribution
- Simple to implement correctly
- Good enough for educational OS
- Easy to debug

**Alternatives considered:**
- Priority-based: More complex, risk of starvation
- CFS (Linux): Overkill for simple OS
- MLFQ: Too complex for learning project

---

## 5. System Calls: INT 0x80

**Decision:** Use software interrupt 0x80 for syscalls

**Why:**
- Traditional Linux approach (before SYSENTER)
- Works on all x86 CPUs
- Easy to implement in assembly
- Clear privilege transition

**Alternative:**
- SYSENTER/SYSEXIT: Faster but more complex setup

---

## 6. Memory Model: Flat 4GB

**Decision:** Flat memory model with 4GB segments

**Why:**
- Simplest protected mode configuration
- All code can access all memory (no segment arithmetic)
- Modern approach (Linux uses same)

**Tradeoff:**
- Requires paging for memory protection
- No hardware segment-based security

---

## 7. Paging: Identity Mapped Kernel

**Decision:** Identity map first 4MB for kernel

**Why:**
- Virtual address = Physical address (simpler debugging)
- Kernel code doesn't need relocation
- Can still use physical addresses directly

**Future improvement:**
- Higher-half kernel (kernel at 0xC0000000+)
- Separate user address spaces

---

## 8. PIC Remapping: IRQs 32-47

**Decision:** Remap master PIC to IRQ 32, slave to IRQ 40

**Why:**
- x86 reserves vectors 0-31 for exceptions
- Default PIC mapping (0-15) conflicts with exceptions
- Industry standard remapping offsets

---

## 9. VGA: Text Mode Only

**Decision:** Use VGA text mode (80x25)

**Why:**
- Simpler than graphics mode
- No framebuffer memory management
- Adequate for shell-based OS
- Fast character output

**Future improvement:**
- VESA graphics mode for GUI
