;; --- Stival1 Header ------------------------------------------------------- ;;

section .stivalehdr

extern __stack_top
extern _kstart_stivale1

align 4
__stivale1_header:
    dq __stack_top
    dw 1
    dw 0
    dw 0
    dw 32
    dq _kstart_stivale1
