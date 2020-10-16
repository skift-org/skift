section .text

;; --- CPU tables ----------------------------------------------------------- ;;

global gdt_flush
gdt_flush:
    lgdt [rdi]
    ret

global idt_flush
idt_flush:
    lidt [rdi]
    ret
