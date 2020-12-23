
global gdt_flush
gdt_flush:
    lgdt [rdi]
    push rbp
    mov rbp, rsp

    push qword 0x10
    push rbp
    pushf
    push qword 0x8
    push .trampoline
    iretq

.trampoline:
    pop rbp

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ax, 0x28
    ltr ax

    ret

global idt_flush
idt_flush:
    lidt [rdi]
    ret

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
