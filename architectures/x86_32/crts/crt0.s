section .text

extern __entry_point

global _start:function (_start.end - _start)
_start:
	call __entry_point
	ud2
.end:
