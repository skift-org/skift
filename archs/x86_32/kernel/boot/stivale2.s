extern __stack_top
extern _kstart_stivale2

__stivale2_framebuffer_tag:
    dq 0x3ecc1bc43d0f7971
    dq 0
    dw 0
    dw 0
    dw 32

__stivale2_write_combining_tag:
    dq 0x4c7bb07731282e00
    dq __stivale2_framebuffer_tag

section .stivale2hdr
align 4
__stivale2_header:
    dq _kstart_stivale2
    dq __stack_top
    dq 0
    dq __stivale2_write_combining_tag
