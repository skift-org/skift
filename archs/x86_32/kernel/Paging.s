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

global paging_load_directory
paging_load_directory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

global paging_invalidate_tlb
paging_invalidate_tlb:
    mov eax, cr3
    mov cr3, eax
    ret
