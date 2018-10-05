; section .bss
; align 16
; stack_bottom:
; resb 16384 ; 16 KiB
; stack_top:

section .text
extern sk_process_exit
global _start:function (_start.end - _start)
_start:
	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems).
	; mov esp, stack_top
	extern main
	call main

	push eax
	call sk_process_exit
.end: