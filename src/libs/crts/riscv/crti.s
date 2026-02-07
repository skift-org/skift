.section .init
.global _init
.type _init, @function
_init:
    addi sp, sp, -16
    sd ra, 8(sp)

.section .fini
.global _fini
.type _fini, @function
_fini:
    addi sp, sp, -16
    sd ra, 8(sp)
