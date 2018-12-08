section .text

extern sk_process_exit
extern stdlib_init
extern program

global _start:function (_start.end - _start)
_start:
	call stdlib_init
	call program

	push eax
	call sk_process_exit
.end: