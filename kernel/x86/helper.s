;; Copyright © 2018-2020 N. Van Bossuyt.                                      ;;
;; This code is licensed under the MIT License.                               ;;
;; See: LICENSE.md                                                            ;;

section .text

global paging_enable
paging_enable:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

global paging_disable
paging_disable:
    mov eax, cr0
    and eax, 0x7fffffff
    mov cr0, eax
    ret

global paging_load_directorie
paging_load_directorie:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

global paging_invalidate_tlb
paging_invalidate_tlb:
    mov eax, cr3
    mov cr3, eax
    ret

# --- CPU tables ------------------------------------------------------------- #

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
    ret

global idt_flush
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

# --- CPUID ------------------------------------------------------------------ #

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
