.globl SwitchContext
.globl InitialSwitchContext
.globl SwitchContextIrq

InitialSwitchContext:
    /* Store the old registers */
    push %r12
    push %r13
    push %r14
    push %r15
    push %rbx
    push %rbp
    mov %rsp, 0(%rdi)

    /* Restore the current registers */
    mov 0(%rsi), %rsp
    pop %rbp
    pop %rbx
    pop %r15
    pop %r14
    pop %r13
    pop %r12

    mov %rdx, %rdi

    ret

SwitchContext:
    /* Store the old registers */
    prefetcht0 (%rsi)
    push %r12
    push %r13
    push %r14
    push %r15
    push %rbx
    push %rbp
    mov %rsp, 0(%rdi)

    /* Restore the current registers */
    mov 0(%rsi), %rsp
    pop %rbp
    pop %rbx
    pop %r15
    pop %r14
    pop %r13
    pop %r12

    ret

SwitchContextIrq:
    /* Store the old registers */
    push %r12
    push %r13
    push %r14
    push %r15
    push %rbx
    push %rbp
    mov %rsp, 0(%rdi)

    /* Restore the current registers */
    mov 0(%rsi), %rsp
    pop %rbp
    pop %rbx
    pop %r15
    pop %r14
    pop %r13
    pop %r12

    /* 3rd param is our function, 4th is context */
    mov %rcx, %rdi
    /* Jump to the start of the ISR */
    jmpq *%rdx

