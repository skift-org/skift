section .text

extern sk_process_exit
extern stdlib_init
extern main

global _start:function (_start.end - _start)
_start:
	call stdlib_init
	call main

	push eax
	call sk_process_exit
.end: