; =============================================================================
; Robust Disk Loading Routine
; Loads sectors from disk using BIOS INT 0x13.
; Includes error checking and retry logic.
; =============================================================================

[bits 16]

; -----------------------------------------------------------------------------
; Function: disk_load
; Purpose: Loads DH sectors to ES:BX from drive DL.
; Input:
;   bx = Destination offset (in ES)
;   dh = Number of sectors to load
;   dl = Drive number (BIOS passes this in DL to bootloader)
; -----------------------------------------------------------------------------
disk_load:
    push dx                 ; Save DX (contains sector count DH)

    ; We try to read from cylinder 0, head 0, sector 2.
    ; Sector 1 is the boot sector itself.
    mov ah, 0x02            ; BIOS read sector function
    mov al, dh              ; Read DH sectors
    mov ch, 0x00            ; Cylinder 0
    mov dh, 0x00            ; Head 0
    mov cl, 0x02            ; Start reading from Sector 2 (1-based)

    int 0x13                ; BIOS interrupt
    jc .disk_error          ; Jump if Carry Flag is set (error)

    pop dx                  ; Restore DX (original requested count)
    cmp dh, al              ; Compare requested (DH) with read (AL)
    jne .sector_error       ; If not equal, we didn't read all sectors

    ret

.disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string_rm
    
    ; Print the error code in AH
    mov dh, ah
    call print_hex_rm
    
    jmp $                   ; Hang

.sector_error:
    mov bx, SECTOR_ERROR_MSG
    call print_string_rm
    jmp $                   ; Hang

; -----------------------------------------------------------------------------
; Data
; -----------------------------------------------------------------------------
DISK_ERROR_MSG:   db "Disk read error! Error Code: ", 0
SECTOR_ERROR_MSG: db "Read incorrect sector count!", 0
