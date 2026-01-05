; cpu/switch.asm - Context Switch
; void context_switch(uint32_t* old_esp, uint32_t new_esp)

[global context_switch]

context_switch:
    ; Save the old registers
    pushad              ; Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    
    ; Get arguments from stack (after pushad, stack layout changed)
    ; [ESP+36] = old_esp pointer
    ; [ESP+40] = new_esp value
    mov eax, [esp+36]   ; EAX = &old_esp
    mov [eax], esp      ; Save current ESP to old process
    
    ; Load new stack
    mov esp, [esp+40]   ; ESP = new_esp
    
    ; Restore new process registers
    popad               ; Pop EDI, ESI, EBP, ESP(ignored), EBX, EDX, ECX, EAX
    
    ret                 ; Return to new process
