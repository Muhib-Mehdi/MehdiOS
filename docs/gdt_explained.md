# Global Descriptor Table (GDT) Explained

In 32-bit Protected Mode, the CPU uses the **Global Descriptor Table (GDT)** to define memory segments. Unlike Real Mode (segment * 16 + offset), Protected Mode uses GDT indices (selectors).

## Flat Memory Model
Modern OSs (Windows, Linux, macOS) generally ignore segmentation in favor of **Paging**. However, x86 requires at least a minimal GDT.
We check for a **Flat Memory Model**:
- **Code Segment**: Base 0x0, Limit 4GB.
- **Data Segment**: Base 0x0, Limit 4GB.

This effectively disables segmentation logic, making `Virtual Address = Linear Address`.

## Descriptor Structure
Each entry is 8 bytes (64 bits).

| Bits | Description | Our Value |
|------|-------------|-----------|
| **Base** | The starting address of the segment | 0x00000000 |
| **Limit** | The size of the segment | 0xFFFFF (4GB with granularity) |
| **Access Byte** | Permissions | 0x9A (Code), 0x92 (Data) |
| **Flags** | Granularity and Size | 0xC (4KB pages, 32-bit) |

### Access Byte Analysis
**Code Segment (0x9A = 10011010b)**
- **Pr** (1): Present.
- **Privl** (00): Ring 0 (Kernel).
- **S** (1): Code/Data segment (not system).
- **Ex** (1): Executable (Code).
- **DC** (0): Non-conforming (can only be called by Ring 0).
- **RW** (1): Readable.
- **Ac** (0): Accessed bit.

**Data Segment (0x92 = 10010010b)**
- **Ex** (0): Not executable (Data).
- **RW** (1): Writable.

## Why do we need this?
Without a valid GDT, the CPU cannot execute in Protected Mode. When we set the PE bit in CR0, the CPU expects CS (Code Segment) and DS (Data Segment) to refer to valid descriptors.
