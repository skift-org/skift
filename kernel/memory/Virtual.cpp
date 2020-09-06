#include <libsystem/thread/Atomic.h>

#include "kernel/memory/Memory.h"
#include "kernel/memory/Virtual.h"
#include "kernel/system/System.h"

#define PD_INDEX(vaddr) ((vaddr) >> 22)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

PageDirectory kpdir __aligned(ARCH_PAGE_SIZE) = {};
PageTable kptable[256] __aligned(ARCH_PAGE_SIZE) = {};

bool virtual_present(PageDirectory *page_directory, uintptr_t virtual_address)
{
    ASSERT_ATOMIC;

    int page_directory_index = PD_INDEX(virtual_address);
    PageDirectoryEntry &page_directory_entry = page_directory->entries[page_directory_index];

    if (!page_directory_entry.Present)
    {
        return false;
    }

    PageTable &page_table = *reinterpret_cast<PageTable *>(page_directory_entry.PageFrameNumber * ARCH_PAGE_SIZE);

    int page_table_index = PT_INDEX(virtual_address);
    PageTableEntry &page_table_entry = page_table.entries[page_table_index];

    if (!page_table_entry.Present)
    {
        return false;
    }

    return true;
}

uintptr_t virtual_to_physical(PageDirectory *page_directory, uintptr_t virtual_address)
{
    ASSERT_ATOMIC;

    int page_directory_index = PD_INDEX(virtual_address);
    PageDirectoryEntry &page_directory_entry = page_directory->entries[page_directory_index];

    if (!page_directory_entry.Present)
    {
        return 0;
    }

    PageTable &page_table = *reinterpret_cast<PageTable *>(page_directory_entry.PageFrameNumber * ARCH_PAGE_SIZE);

    int page_table_index = PT_INDEX(virtual_address);
    PageTableEntry &page_table_entry = page_table.entries[page_table_index];

    if (!page_table_entry.Present)
    {
        return 0;
    }

    return (page_table_entry.PageFrameNumber * ARCH_PAGE_SIZE) + (virtual_address & 0xfff);
}

Result virtual_map(PageDirectory *page_directory, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags)
{
    ASSERT_ATOMIC;

    for (size_t i = 0; i < physical_range.size() / ARCH_PAGE_SIZE; i++)
    {
        size_t offset = i * ARCH_PAGE_SIZE;

        int page_directory_index = PD_INDEX(virtual_address + offset);
        PageDirectoryEntry &page_directory_entry = page_directory->entries[page_directory_index];
        PageTable *page_table = reinterpret_cast<PageTable *>(page_directory_entry.PageFrameNumber * ARCH_PAGE_SIZE);

        if (!page_directory_entry.Present)
        {
            Result alloc_result = memory_alloc_identity(page_directory, MEMORY_CLEAR, (uintptr_t *)&page_table);

            if (alloc_result != SUCCESS)
            {
                return alloc_result;
            }

            page_directory_entry.Present = 1;
            page_directory_entry.Write = 1;
            page_directory_entry.User = 1;
            page_directory_entry.PageFrameNumber = (uint32_t)(page_table) >> 12;
        }

        int page_table_index = PT_INDEX(virtual_address + offset);
        PageTableEntry &page_table_entry = page_table->entries[page_table_index];

        page_table_entry.Present = 1;
        page_table_entry.Write = 1;
        page_table_entry.User = flags & MEMORY_USER;
        page_table_entry.PageFrameNumber = (physical_range.base() + offset) >> 12;
    }

    paging_invalidate_tlb();

    return SUCCESS;
}

MemoryRange virtual_alloc(PageDirectory *page_directory, MemoryRange physical_range, MemoryFlags flags)
{
    ASSERT_ATOMIC;

    bool is_user_memory = flags & MEMORY_USER;

    uintptr_t virtual_address = 0;
    size_t current_size = 0;

    // we skip the first page to make null deref trigger a page fault
    for (size_t i = (is_user_memory ? 256 : 1) * 1024; i < (is_user_memory ? 1024 : 256) * 1024; i++)
    {
        uintptr_t current_address = i * ARCH_PAGE_SIZE;

        if (!virtual_present(page_directory, current_address))
        {
            if (current_size == 0)
            {
                virtual_address = current_address;
            }

            current_size += ARCH_PAGE_SIZE;

            if (current_size == physical_range.size())
            {
                virtual_map(page_directory, physical_range, virtual_address, flags);

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
    ASSERT_ATOMIC;

    for (size_t i = 0; i < virtual_range.size() / ARCH_PAGE_SIZE; i++)
    {
        size_t offset = i * ARCH_PAGE_SIZE;

        size_t page_directory_index = PD_INDEX(virtual_range.base() + offset);
        PageDirectoryEntry *page_directory_entry = &page_directory->entries[page_directory_index];

        PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * ARCH_PAGE_SIZE);

        size_t page_table_index = PT_INDEX(virtual_range.base() + offset);
        PageTableEntry *page_table_entry = &page_table->entries[page_table_index];

        if (page_table_entry->Present)
            page_table_entry->as_uint = 0;
    }

    paging_invalidate_tlb();
}
