section .text

global __enterKernel
__enterKernel:
    ; rcx : entry to jump to
    ; rdx : payload to pass
    ; r8 : stack to use
    ; r9 : vmm context
    cli
    mov rdi, 0xC001B001
    mov rsi, rdx
    mov cr3, r9
    mov rsp, r8
    mov rbp, 0

    call rcx
