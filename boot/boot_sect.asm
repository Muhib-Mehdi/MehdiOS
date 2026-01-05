; =============================================================================
; Boot Sector (MBR)
; The BIOS loads this 512-byte sector at 0x7c00.
; It initializes the stack, loads the kernel from disk, and switches to 32-bit mode.
; =============================================================================
[org 0x7c00]

    ; -------------------------------------------------------------------------
    ; Initialization
    ; -------------------------------------------------------------------------
    mov [BOOT_DRIVE], dl ; BIOS stores our boot drive in DL, so it's best to save this.

    ; Setup the stack (growing downwards from 0x8000)
    mov bp, 0x8000
    mov sp, bp

    ; Print welcome message
    mov bx, MSG_REAL_MODE
    call print_string_rm

    ; -------------------------------------------------------------------------
    ; Load Kernel
    ; -------------------------------------------------------------------------
    call load_kernel

    ; -------------------------------------------------------------------------
    ; Switch to Protected Mode
    ; -------------------------------------------------------------------------
    call switch_to_pm

    jmp $ ; Infinite loop (should never happen)

; -----------------------------------------------------------------------------
; Real Mode Includes
; -----------------------------------------------------------------------------
%include "boot/print_real_mode.asm"
%include "boot/disk_load.asm"
%include "boot/gdt.asm"
%include "boot/switch_pm.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string_rm

    ; Load KERNEL_SECTORS from disk to memory at KERNEL_OFFSET
    ; We are loading to ES:BX = 0x0000:0x1000
    ; Adjust KERNEL_SECTORS in Makefile based on kernel size
    mov bx, KERNEL_OFFSET
    mov dh, 50            ; Load 50 sectors (approx 25KB)
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
; -----------------------------------------------------------------------------
; Protected Mode Entry Point
; -----------------------------------------------------------------------------
BEGIN_PM:
    ; Jump to the C Kernel Entry Point
    ; The linker script places the kernel entry code at KERNEL_OFFSET
    jmp KERNEL_OFFSET

    jmp $ ; Hang if kernel returns

; -----------------------------------------------------------------------------
; Data
; -----------------------------------------------------------------------------
BOOT_DRIVE:      db 0
MSG_REAL_MODE:   db "Started in 16-bit Real Mode", 0x0d, 0x0a, 0
MSG_LOAD_KERNEL: db "Loading kernel into memory...", 0x0d, 0x0a, 0
KERNEL_OFFSET equ 0x1000

; -----------------------------------------------------------------------------
; Boot Sector Padding
; -----------------------------------------------------------------------------
times 510-($-$$) db 0   ; Padding to 510 bytes
dw 0xaa55               ; Boot Magic Number
