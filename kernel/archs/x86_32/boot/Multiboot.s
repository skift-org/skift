;; --- kernel entry point --------------------------------------------------- ;;

extern __stack_top

section .text

global _kstart:function (_kstart.end - _kstart)
_kstart:
	cli
	cld

	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems).
	mov esp, __stack_top
	xor ebp, ebp

	push eax ; Push the multiboot magic
	push ebx ; Push the multiboot header adress.

	extern arch_x86_32_main
	call arch_x86_32_main

	cli

.hang:
	hlt
	jmp .hang

.end:
