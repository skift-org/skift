section .text

extern __entryPoint

global _start:function (_start.end - _start)
_start:
    call __entryPoint
    ud2
.end:
