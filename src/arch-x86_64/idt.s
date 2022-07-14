section .text
global _idtLoad
_idtLoad:
    lidt  [rdi]
    ret
