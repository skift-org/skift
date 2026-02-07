.section .text
.global _start
.type _start, @function

.extern __entryPoint

_start:
    .option push
    .option norelax
    1: auipc gp, %pcrel_hi(__global_pointer$)
    addi  gp, gp, %pcrel_lo(1b)
    .option pop
    call __entryPoint
    unimp
    .size _start, .-_start
