; =============================================================================
; 16-bit Real Mode Printing Utilities
; Used by the bootloader before switching to 32-bit Protected Mode.
; =============================================================================

[bits 16]

; -----------------------------------------------------------------------------
; Function: print_string_rm
; Purpose: Prints a null-terminated string to the BIOS teletype output.
; Input: bx = Address of the string to print
; -----------------------------------------------------------------------------
print_string_rm:
    pusha                   ; Save all registers

.loop:
    mov al, [bx]            ; Load character from address in bx
    cmp al, 0               ; Check for null terminator
    je .done                ; If 0, we are done

    mov ah, 0x0e            ; BIOS teletype function (TTY)
    int 0x10                ; BIOS interrupt to print char in AL

    add bx, 1               ; Increment pointer
    jmp .loop               ; Repeat

.done:
    popa                    ; Restore all registers
    ret

; -----------------------------------------------------------------------------
; Function: print_hex_rm
; Purpose: Prints a 16-bit value in hexadecimal format (e.g., 0x1234).
; Input: dx = The 16-bit value to print
; -----------------------------------------------------------------------------
print_hex_rm:
    pusha

    mov bx, HEX_OUT         ; Load template string "0x0000"
    
    ; We need to fill the last 4 characters of HEX_OUT with the hex digits
    ; HEX_OUT is at [bx]. The string is '0', 'x', char1, char2, char3, char4, 0
    ; So char1 is at bx+2, char2 at bx+3, etc.

    ; Index 3 (4th nibble)
    mov ax, dx
    and ax, 0x000f          ; Mask last 4 bits
    call .hex_convert
    mov [bx + 5], al        ; Store at last position

    ; Index 2
    mov ax, dx
    shr ax, 4
    and ax, 0x000f
    call .hex_convert
    mov [bx + 4], al

    ; Index 1
    mov ax, dx
    shr ax, 8
    and ax, 0x000f
    call .hex_convert
    mov [bx + 3], al

    ; Index 0
    mov ax, dx
    shr ax, 12
    and ax, 0x000f
    call .hex_convert
    mov [bx + 2], al

    mov bx, HEX_OUT
    call print_string_rm

    popa
    ret

.hex_convert:
    ; Converts value in AL (0-15) to ASCII ('0'-'9' or 'A'-'F')
    cmp al, 9
    jg .is_letter
    add al, '0'
    ret
.is_letter:
    add al, 'A' - 10
    ret

; Template string for hex output
HEX_OUT: db '0x0000', 0
