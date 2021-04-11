;; --- kernel stack --------------------------------------------------------- ;;

section .bss
align 16

global __stack_bottom
global __stack_top

__stack_bottom:
resb 16384 ; 16 KiB
__stack_top:
