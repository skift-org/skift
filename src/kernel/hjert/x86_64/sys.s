section .text

extern _sysDispatch

%macro push_all 0

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

%endmacro

%macro pop_all 0

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx

%endmacro

ALIGN 4096
global _sysHandler
_sysHandler:
    swapgs               ; swap from USER gs to KERNEL gs
    mov [gs:0x8], rsp    ; save current stack to the local cpu structure
    mov rsp, [gs:0x0]    ; use the kernel syscall stack

    sti

    push qword 0x1b      ; user data
    push qword [gs:0x8]  ; saved stack
    push r11             ; saved rflags
    push qword 0x18      ; user code
    push rcx             ; current IP

    push qword 0
    push qword 0

    cld
    push_all

    mov rdi, rsp
    mov rbp, 0

    call _sysDispatch

    pop_all              ; pop everything except rax because we use it for the return value

    cli
    mov rsp, [gs:0x8]    ; return to the user stack
    swapgs
    o64 sysret
