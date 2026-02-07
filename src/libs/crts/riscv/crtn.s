.section .init
    ld ra, 8(sp)
    addi sp, sp, 16
    ret

.section .fini
    ld ra, 8(sp)
    addi sp, sp, 16
    ret
