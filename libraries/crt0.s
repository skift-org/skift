section .text

extern __plug_init
extern __plug_fini
extern main
extern process_exit

global _start:function (_start.end - _start)
_start:
	call __plug_init

	call main

	push eax
	call __plug_fini
.end:
