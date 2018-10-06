section .text
extern sk_process_exit
global _start:function (_start.end - _start)
_start:
	extern main
	call main

	push eax
	call sk_process_exit
.end: