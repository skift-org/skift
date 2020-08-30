section .text

;; --- CPU tables ----------------------------------------------------------- ;;

global gdt_flush
gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax
    jmp 0x08:._gdt_flush

._gdt_flush:
    mov ax, 0x28
    ltr ax
    ret

global idt_flush
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret


;; --- CPUID ---------------------------------------------------------------- ;;

global cpuid_get_feature_EDX
cpuid_get_feature_EDX:
    mov eax, 1
    cpuid
    mov eax, edx
    ret

global cpuid_get_feature_ECX
cpuid_get_feature_ECX:
    mov eax, 1
    cpuid
    mov eax, ecx
    ret
