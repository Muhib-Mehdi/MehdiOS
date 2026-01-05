; =============================================================================
; Global Descriptor Table (GDT)
; Defines the memory segments for the kernel in Protected Mode.
; We use a 'Flat Memory Model' where both Code and Data segments cover the entire
; 4GB address space (0x00000000 to 0xFFFFFFFF).
; =============================================================================

gdt_start:

; -----------------------------------------------------------------------------
; Null Descriptor
; The first descriptor in the GDT must be null (8 bytes of 0).
; -----------------------------------------------------------------------------
gdt_null:
    dd 0x0
    dd 0x0

; -----------------------------------------------------------------------------
; Kernel Code Segment Descriptor
; Base: 0x0, Limit: 0xFFFFF, Granularity: 4KB (Flag=1) -> 4GB total
; -----------------------------------------------------------------------------
gdt_code:
    ; Limit (bits 0-15): 0xffff
    dw 0xffff
    ; Base (bits 0-15): 0x0
    dw 0x0000
    ; Base (bits 16-23): 0x0
    db 0x00
    ; Access Byte: 
    ;   Pr (Present) = 1
    ;   Privl (Privilege) = 00 (Ring 0)
    ;   S (Descriptor Type) = 1 (Code/Data)
    ;   Ex (Executable) = 1 (Code)
    ;   DC (Direction/Conforming) = 0
    ;   RW (Readable) = 1
    ;   Ac (Accessed) = 0
    ;   -> 10011010b = 0x9a
    db 0x9a
    ; Flags (4 bits) + Limit (top 4 bits)
    ;   Gr (Granularity) = 1 (4KB blocks)
    ;   Sz (Size) = 1 (32-bit protected mode)
    ;   0, 0
    ;   Limit (16-19) = 0xF
    ;   -> 11001111b = 0xcf
    db 0xcf
    ; Base (bits 24-31): 0x0
    db 0x00

; -----------------------------------------------------------------------------
; Kernel Data Segment Descriptor
; Same range as Code segment, but non-executable and writable.
; -----------------------------------------------------------------------------
gdt_data:
    ; Limit (0-15)
    dw 0xffff
    ; Base (0-15)
    dw 0x0000
    ; Base (16-23)
    db 0x00
    ; Access Byte:
    ;   Pr=1, Priv=00, S=1, Ex=0 (Data), DC=0, RW=1 (Writable), Ac=0
    ;   -> 10010010b = 0x92
    db 0x92
    ; Flags + Limit (same as code)
    db 0xcf
    ; Base (24-31)
    db 0x00

; -----------------------------------------------------------------------------
; User Code Segment Descriptor (Ring 3)
; Same as kernel code but DPL = 3
; -----------------------------------------------------------------------------
gdt_user_code:
    dw 0xffff         ; Limit (0-15)
    dw 0x0000         ; Base (0-15)
    db 0x00           ; Base (16-23)
    ; Access Byte: Pr=1, DPL=11 (Ring 3), S=1, Ex=1, DC=0, RW=1, Ac=0
    ; -> 11111010b = 0xFA
    db 0xFA
    db 0xcf           ; Flags + Limit high
    db 0x00           ; Base (24-31)

; -----------------------------------------------------------------------------
; User Data Segment Descriptor (Ring 3)
; Same as kernel data but DPL = 3
; -----------------------------------------------------------------------------
gdt_user_data:
    dw 0xffff         ; Limit (0-15)
    dw 0x0000         ; Base (0-15)
    db 0x00           ; Base (16-23)
    ; Access Byte: Pr=1, DPL=11, S=1, Ex=0, DC=0, RW=1, Ac=0
    ; -> 11110010b = 0xF2
    db 0xF2
    db 0xcf           ; Flags + Limit high
    db 0x00           ; Base (24-31)

; -----------------------------------------------------------------------------
; TSS Descriptor (Will be filled in by C code)
; Base and Limit will be set at runtime
; -----------------------------------------------------------------------------
gdt_tss:
    dw 0x0067         ; Limit (sizeof(tss_entry_t) - 1 = 103 bytes)
    dw 0x0000         ; Base (0-15) - filled by C
    db 0x00           ; Base (16-23) - filled by C
    ; Access Byte: Pr=1, DPL=00, S=0 (System), Type=1001 (Available TSS)
    ; -> 10001001b = 0x89
    db 0x89
    db 0x00           ; Flags (0) + Limit high (0)
    db 0x00           ; Base (24-31) - filled by C

gdt_end:

; -----------------------------------------------------------------------------
; GDT Descriptor
; This structure is loaded into the GDTR register.
; -----------------------------------------------------------------------------
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size of GDT (limit)
    dd gdt_start               ; Start address of GDT

; -----------------------------------------------------------------------------
; Segment Constants
; We export these offsets so other assembly files can use them.
; -----------------------------------------------------------------------------
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
USER_CODE_SEG equ gdt_user_code - gdt_start
USER_DATA_SEG equ gdt_user_data - gdt_start
TSS_SEG equ gdt_tss - gdt_start
