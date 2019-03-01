section .text

extern main

global _start:function (_start.end - _start)
_start:
	call main
.end: