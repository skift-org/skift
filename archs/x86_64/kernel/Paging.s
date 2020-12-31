global paging_load_directory
paging_load_directory:
    mov cr3, rdi
    ret

global paging_invalidate_tlb
paging_invalidate_tlb:
    mov rax, cr3
    mov cr3, rax
    ret
