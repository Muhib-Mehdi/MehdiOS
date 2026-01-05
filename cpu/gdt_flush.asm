; cpu/gdt_flush.asm - Load GDT and TSS
[global gdt_flush]
[global tss_flush]

gdt_flush:
    mov eax, [esp+4]  ; Get pointer to GDT descriptor
    lgdt [eax]        ; Load GDT
    
    ; Reload segment registers
    mov ax, 0x10      ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Far jump to reload CS
    jmp 0x08:.flush
.flush:
    ret

tss_flush:
    mov ax, 0x28      ; TSS segment selector (index 5 * 8 = 40 = 0x28)
    ltr ax            ; Load Task Register
    ret
