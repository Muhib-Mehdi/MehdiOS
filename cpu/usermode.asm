; cpu/usermode.asm - Switch to Ring 3 (User Mode)
[global switch_to_user_mode]
[extern user_program]

; User mode segment selectors (with RPL=3)
USER_CS equ 0x1B  ; 0x18 | 0x3
USER_DS equ 0x23  ; 0x20 | 0x3

switch_to_user_mode:
    cli
    
    ; Set up data segments for user mode
    mov ax, USER_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Build the iret stack frame:
    ; SS, ESP, EFLAGS, CS, EIP (pushed in reverse order)
    
    ; Push SS (user data segment)
    push dword USER_DS
    
    ; Push ESP (user stack)
    push dword 0x50000
    
    ; Push EFLAGS (with IF set for interrupts)
    pushfd
    pop eax
    or eax, 0x200       ; Set IF (interrupt flag)
    push eax
    
    ; Push CS (user code segment)
    push dword USER_CS
    
    ; Push EIP (address of user program)
    push dword user_program
    
    ; Return to user mode!
    iret
