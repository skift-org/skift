;; --- kernel entry point --------------------------------------------------- ;;

section .bss
align 16

global __stack_bottom
global __stack_top

__stack_bottom:
resb 16384
__stack_top:

section .text

global _kstart
_kstart:
	cli
	cld

	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems).
	mov esp, __stack_top
	xor ebp, ebp

	push eax ; Push the multiboot magic
	push ebx ; Push the multiboot header adress.

	extern kmain
	call kmain

	cli

.hang:
	hlt
	jmp .hang

.end:

