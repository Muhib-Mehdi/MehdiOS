# Virtual Memory in MehdiOS

## Overview
MehdiOS implements x86 paging for virtual memory support.

## Memory Addressing with Paging

```
Virtual Address (32 bits)
┌────────────┬────────────┬──────────────┐
│ Dir Index  │ Table Index│    Offset    │
│  (10 bits) │  (10 bits) │  (12 bits)   │
└────────────┴────────────┴──────────────┘
      │             │            │
      │             │            └─► Byte offset within page (0-4095)
      │             └──────────────► Page table entry (0-1023)
      └────────────────────────────► Page directory entry (0-1023)
```

## Translation Process

```
Virtual Address: 0x00401234
                    │
                    ▼
┌─────────────────────────────────────┐
│ 1. Extract Directory Index: 0x001  │
│ 2. Extract Table Index: 0x001      │
│ 3. Extract Offset: 0x234           │
└─────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────┐
│ CR3 → Page Directory               │
│  [0x001] → Page Table Address      │
└─────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────┐
│ Page Table                          │
│  [0x001] → Physical Frame Address  │
└─────────────────────────────────────┘
                    │
                    ▼
Physical Address = Frame + Offset
```

## Page Entry Flags

| Bit | Name | Description |
|-----|------|-------------|
| 0 | PRESENT | Page is in physical memory |
| 1 | WRITE | Page is writable |
| 2 | USER | Accessible from Ring 3 |
| 5 | ACCESSED | Page was read |
| 6 | DIRTY | Page was written |

## Identity Mapping

MehdiOS identity maps the first 4MB:
- Virtual 0x00000000 → Physical 0x00000000
- Virtual 0x003FFFFF → Physical 0x003FFFFF

This covers:
- Kernel code and data
- VGA memory (0xB8000)
- Stack and heap

## Page Fault Handler

When a page fault occurs (INT 14), the handler:
1. Reads faulting address from CR2
2. Decodes error code (present, write, user)
3. Currently halts (future: allocate missing page)

## Enabling Paging

```c
// In kernel.c, uncomment:
paging_init();
```

> [!CAUTION]
> Paging is **disabled by default** for stability.
> Enable only after thorough testing.
