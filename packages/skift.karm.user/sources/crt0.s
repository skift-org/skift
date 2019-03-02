section .text

extern __plug_init
extern main
extern sk_process_exit

global _start:function (_start.end - _start)
_start:
	call __plug_init
	call main

	push eax
	call sk_process_exit
.end: