; =============================================================================
; Mode Switch Routine
; Switches the CPU from 16-bit Mode to 32-bit Protected Mode.
; =============================================================================

[bits 16]

switch_to_pm:
    ; 1. Disable Interrupts
    ; We must turn off interrupts because the IDT (Interrupt Descriptor Table)
    ; pointing to Real Mode ISRs will be invalid in Protected Mode.
    cli

    ; 2. Load the GDT
    ; Tell the CPU about our global descriptor table.
    lgdt [gdt_descriptor]

    ; 3. Enable Protected Mode
    ; Set the PE (Protection Enable) bit in control register CR0.
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; 4. Far Jump
    ; We must perform a far jump to flush the CPU pipeline.
    ; This clears any 16-bit instructions that were prefetched.
    ; It also sets the CS register to our new CODE_SEG.
    jmp CODE_SEG:init_pm

[bits 32]
; We are now in 32-bit Protected Mode!
init_pm:
    ; 5. Update Segment Registers
    ; The other segment registers (DS, SS, ES, FS, GS) must point to our DATA segment.
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 6. Update Stack
    ; Move the stack to a safe location at the top of free memory.
    mov ebp, 0x90000
    mov esp, ebp

    ; 7. Return to main flow
    call BEGIN_PM
