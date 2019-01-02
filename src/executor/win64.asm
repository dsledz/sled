.CODE

InitialSwitchContext PROC
    mov [rcx], rbx
    mov [rcx + 8], rdi
    mov [rcx + 16], rsi
    mov [rcx + 24], r12
    mov [rcx + 32], r13
    mov [rcx + 40], r14
    mov [rcx + 48], r15
    mov [rcx + 56], rsp
    mov [rcx + 64], rbp
    mov rax, gs:[0]
    mov [rcx + 72], rax
    mov rax, gs:[8]
    mov [rcx + 80], rax
    mov rax, gs:[16]
    mov [rcx + 88], rax

    mov rbx,  [rdx]
    mov rdi,  [rdx + 8]
    mov rsi, [rdx + 16]
    mov r12, [rdx + 24]
    mov r13, [rdx + 32]
    mov r14, [rdx + 40]
    mov r15, [rdx + 48]
    mov rsp, [rdx + 56]
    mov rbp, [rdx + 64]
    mov rax, 0ffffffffffffffffh
    mov gs:[0], rax
    mov rax, [rdx + 56]
    mov gs:[8], rax
    ; XXX Figure out the current size
    sub rax, 010000h
    mov gs:[16], rax
    mov rcx, r8

    ret
InitialSwitchContext ENDP

SwitchContext PROC
    mov [rcx], rbx
    mov [rcx + 8], rdi
    mov [rcx + 16], rsi
    mov [rcx + 24], r12
    mov [rcx + 32], r13
    mov [rcx + 40], r14
    mov [rcx + 48], r15
    mov [rcx + 56], rsp
    mov [rcx + 64], rbp
    mov rax, gs:[0]
    mov [rcx + 72], rax
    mov rax, gs:[8]
    mov [rcx + 80], rax
    mov rax, gs:[16]
    mov [rcx + 88], rax

    mov rbx,  [rdx]
    mov rdi,  [rdx + 8]
    mov rsi, [rdx + 16]
    mov r12, [rdx + 24]
    mov r13, [rdx + 32]
    mov r14, [rdx + 40]
    mov r15, [rdx + 48]
    mov rsp, [rdx + 56]
    mov rbp, [rdx + 64]
    mov rax, [rdx + 72]
    mov gs:[0], rax
    mov rax, [rdx + 80]
    mov gs:[8], rax
    mov rax, [rdx + 88]
    mov gs:[16], rax

    ret
SwitchContext ENDP

SwitchContextIrq PROC FRAME
    mov [rcx], rbx
    mov [rcx + 8], rdi
    mov [rcx + 16], rsi
    mov [rcx + 24], r12
    mov [rcx + 32], r13
    mov [rcx + 40], r14
    mov [rcx + 48], r15
    mov [rcx + 56], rsp
    mov [rcx + 64], rbp
    mov rax, gs:[0]
    mov [rcx + 72], rax
    mov rax, gs:[8]
    mov [rcx + 80], rax
    mov rax, gs:[16]
    mov [rcx + 88], rax

    mov rbx,  [rdx]
    mov rdi,  [rdx + 8]
    mov rsi, [rdx + 16]
    mov r12, [rdx + 24]
    mov r13, [rdx + 32]
    mov r14, [rdx + 40]
    mov r15, [rdx + 48]
    mov rsp, [rdx + 56]
    mov rbp, [rdx + 64]
    mov rax, [rdx + 72]
    mov gs:[0], rax
    mov rax, [rdx + 80]
    mov gs:[8], rax
    mov rax, [rdx + 88]
    mov gs:[16], rax

    push rbp
.PUSHREG rbp
    sub rsp, 040h
.ALLOCSTACK 040h
    lea rbp, [rsp+020h]
.SETFRAME rbp, 020h
.ENDPROLOG

    mov rcx, r9
    call r8

    lea rsp, [rbp-020h]
    add rsp, 040h
    pop rbp
    ret
SwitchContextIrq ENDP

NoSwitchIrq PROC FRAME
    push rbp
.PUSHREG rbp
    sub rsp, 040h
.ALLOCSTACK 040h
.ENDPROLOG

    mov rcx, r9
    call r8

    add rsp, 040h
    pop rbp
    ret
NoSwitchIrq ENDP

NoSwitchIrq2 PROC
    mov rcx, r9
    jmp r8
NoSwitchIrq2 ENDP

END
