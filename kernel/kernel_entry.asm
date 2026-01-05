; =============================================================================
; Kernel Entry Point
; Ensures we jump into C code at the right place.
; =============================================================================

[bits 32]
[extern main] ; Define calling point. Must have same name as kernel.c 'main' function
global _start ; This is not strictly necessary for custom linker script but good practice

_start:
    call main ; Call the C function
    jmp $     ; Hang forever when we return from the kernel
