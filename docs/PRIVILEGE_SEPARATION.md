# Privilege Separation in MehdiOS

## Overview
MehdiOS implements CPU privilege levels (rings) for security separation between kernel and user code.

## CPU Privilege Rings

```
┌─────────────────────────────────────────────────────────────┐
│                        Ring 3 (User Mode)                   │
│    ┌─────────────────────────────────────────────────────┐  │
│    │                  User Programs                       │  │
│    │  - Cannot access hardware directly                   │  │
│    │  - Cannot execute privileged instructions            │  │
│    │  - Can only interact via system calls (INT 0x80)     │  │
│    └─────────────────────────────────────────────────────┘  │
│                              ▲                               │
│                              │ System Call (INT 0x80)        │
│                              ▼                               │
│    ┌─────────────────────────────────────────────────────┐  │
│    │                 Ring 0 (Kernel Mode)                 │  │
│    │  - Full hardware access                              │  │
│    │  - Can execute ANY instruction                       │  │
│    │  - Manages memory, processes, devices                │  │
│    └─────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## GDT Segment Configuration

| Selector | Name | Base | Limit | DPL | Purpose |
|----------|------|------|-------|-----|---------|
| 0x00 | Null | 0 | 0 | - | Required |
| 0x08 | Kernel CS | 0 | 4GB | 0 | Kernel code |
| 0x10 | Kernel DS | 0 | 4GB | 0 | Kernel data |
| 0x18 | User CS | 0 | 4GB | 3 | User code |
| 0x20 | User DS | 0 | 4GB | 3 | User data |
| 0x28 | TSS | tss_entry | 104 | 0 | Task State |

## System Call Flow (INT 0x80)

```
User Program (Ring 3)
    │
    │ int 0x80 (syscall number in EAX)
    ▼
┌─────────────────────────────────────┐
│ CPU Privilege Check                  │
│ - Checks IDT gate DPL (must be 3)   │
│ - Switches to kernel stack (TSS)    │
│ - Saves user SS:ESP, EFLAGS, CS:EIP │
└─────────────────────────────────────┘
    │
    ▼
Kernel Handler (Ring 0)
    │
    ├─ isr128 (cpu/interrupt.asm)
    │   └─ Saves registers
    │   └─ Calls isr_handler()
    │
    ├─ syscall_callback (kernel/syscall.c)
    │   └─ Dispatches based on EAX
    │
    └─ sys_write(), sys_exit(), etc.
    │
    ▼
IRET (Return to User Mode)
    │
    └─ Restores SS:ESP, EFLAGS, CS:EIP
    └─ CPU switches back to Ring 3
```

## Task State Segment (TSS)

The TSS stores the kernel stack pointer for Ring 0.
When a syscall occurs, CPU loads ESP from `tss.esp0`.

```c
typedef struct {
    uint32_t prev_tss;
    uint32_t esp0;      // Kernel stack pointer ← Critical!
    uint32_t ss0;       // Kernel stack segment
    // ... other fields
} tss_entry_t;
```

## Implemented System Calls

| Number | Name | Arguments | Description |
|--------|------|-----------|-------------|
| 0 | exit | code | Terminate process |
| 1 | write | fd, buf, len | Write to screen |
| 2 | read | fd, buf, len | Read from keyboard |

## Security Implications

**What Ring 3 CANNOT do:**
- Access I/O ports (IN/OUT instructions)
- Modify page tables
- Disable interrupts (CLI)
- Access kernel memory (if paging enabled)

**Why this matters for MIT:**
- Demonstrates understanding of hardware security
- Shows how OS protects itself from user programs
- Foundation for multi-user systems
