#include "kernel/memory/Virtual.h"
#include "kernel/memory/Memory.h"
#include "kernel/system/System.h"

#define PD_INDEX(vaddr) ((vaddr) >> 22)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

PageDirectory kpdir __aligned(PAGE_SIZE) = {};
PageTable kptable[256] __aligned(PAGE_SIZE) = {};

bool virtual_present(PageDirectory *page_directory, uintptr_t virtual_address)
{
    int page_directory_index = PD_INDEX(virtual_address);
    int page_table_index = PT_INDEX(virtual_address);

    PageDirectoryEntry *page_directory_entry = &page_directory->entries[page_directory_index];

    if (!page_directory_entry->Present)
    {
        return false;
    }

    PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * PAGE_SIZE);
    PageTableEntry *page_table_entry = &page_table->entries[page_table_index];

    if (!page_table_entry->Present)
    {
        return false;
    }

    return true;
}

uintptr_t virtual_to_physical(PageDirectory *page_directory, uintptr_t virtual_address)
{
    int page_directory_index = PD_INDEX(virtual_address);
    int page_table_index = PT_INDEX(virtual_address);

    PageDirectoryEntry *page_directory_entry = &page_directory->entries[page_directory_index];
    PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * PAGE_SIZE);
    PageTableEntry *page_table_entry = &page_table->entries[page_table_index];

    return (page_table_entry->PageFrameNumber * PAGE_SIZE) + (virtual_address & 0xfff);
}

int virtual_map(PageDirectory *pdir, uint vaddr, uint paddr, uint count, bool user)
{
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);

        PageDirectoryEntry *pde = &pdir->entries[pdi];
        PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);

        if (!pde->Present)
        {
            memory_alloc_identity(pdir, MEMORY_CLEAR, (uintptr_t *)&ptable);

            pde->Present = 1;
            pde->Write = 1;
            pde->User = user;
            pde->PageFrameNumber = (u32)(ptable) >> 12;
        }

        PageTableEntry *p = &ptable->entries[pti];

        p->Present = 1;
        p->Write = 1;
        p->User = user;
        p->PageFrameNumber = (paddr + offset) >> 12;
    }

    paging_invalidate_tlb();

    return 0;
}

MemoryRange virtual_alloc(PageDirectory *pdir, MemoryRange physical_range, MemoryFlags flags)
{
    bool is_user_memory = flags & MEMORY_USER;

    uintptr_t virtual_address = 0;
    size_t current_size = 0;

    // we skip the first page to make null deref trigger a page fault
    for (size_t i = (is_user_memory ? 256 : 1) * 1024; i < (is_user_memory ? 1024 : 256) * 1024; i++)
    {
        uintptr_t current_address = i * PAGE_SIZE;

        if (!virtual_present(pdir, current_address))
        {
            if (current_size == 0)
            {
                virtual_address = current_address;
            }

            current_size += PAGE_SIZE;

            if (current_size == physical_range.size)
            {
                virtual_map(pdir, virtual_address, physical_range.base, physical_range.size / PAGE_SIZE, is_user_memory);
                return (MemoryRange){virtual_address, current_size};
            }
        }
        else
        {
            current_size = 0;
        }
    }

    system_panic("Out of virtual memory!");
}

void virtual_free(PageDirectory *page_directory, MemoryRange virtual_range)
{
    for (size_t i = 0; i < virtual_range.size / PAGE_SIZE; i++)
    {
        size_t offset = i * PAGE_SIZE;

        size_t page_directory_index = PD_INDEX(virtual_range.base + offset);
        size_t page_table_index = PT_INDEX(virtual_range.base + offset);

        PageDirectoryEntry *page_directory_entry = &page_directory->entries[page_directory_index];
        PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * PAGE_SIZE);
        PageTableEntry *page_table_entry = &page_table->entries[page_table_index];

        if (page_table_entry->Present)
            page_table_entry->as_uint = 0;
    }

    paging_invalidate_tlb();
}
