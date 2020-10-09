section .text

extern main
extern __plug_init
extern __plug_fini

global _start
_start:
    call __plug_init
    call main
    mov rax, rdi
    call __plug_fini
