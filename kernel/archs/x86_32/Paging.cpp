#include <string.h>

#include "system/Streams.h"
#include <libutils/ResultOr.h>

#include "archs/Arch.h"
#include "archs/x86_32/Paging.h"

#include "system/interrupts/Interupts.h"
#include "system/memory/Memory.h"
#include "system/memory/Physical.h"
#include "system/system/System.h"

namespace Arch::x86_32
{

PageDirectory _kernel_page_directory ALIGNED(ARCH_PAGE_SIZE) = {};
PageTable _kernel_page_tables[256] ALIGNED(ARCH_PAGE_SIZE) = {};

void virtual_initialize()
{
    // Setup the kernel pagedirectory.
    for (size_t i = 0; i < 256; i++)
    {
        PageDirectoryEntry *entry = &_kernel_page_directory.entries[i];
        entry->User = 0;
        entry->Write = 1;
        entry->Present = 1;
        entry->PageFrameNumber = (size_t)&_kernel_page_tables[i] / ARCH_PAGE_SIZE;
    }
}

void virtual_memory_enable()
{
    paging_enable();
}

PageDirectory *kernel_page_directory()
{
    return &_kernel_page_directory;
}

bool virtual_present(PageDirectory *page_directory, uintptr_t virtual_address)
{
    ASSERT_INTERRUPTS_RETAINED();

    int page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    PageDirectoryEntry &page_directory_entry = page_directory->entries[page_directory_index];

    if (!page_directory_entry.Present)
    {
        return false;
    }

    PageTable &page_table = *reinterpret_cast<PageTable *>(page_directory_entry.PageFrameNumber * ARCH_PAGE_SIZE);

    int page_table_index = PAGE_TABLE_INDEX(virtual_address);
    PageTableEntry &page_table_entry = page_table.entries[page_table_index];

    return page_table_entry.Present;
}

uintptr_t virtual_to_physical(PageDirectory *page_directory, uintptr_t virtual_address)
{
    ASSERT_INTERRUPTS_RETAINED();

    int page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    PageDirectoryEntry &page_directory_entry = page_directory->entries[page_directory_index];

    if (!page_directory_entry.Present)
    {
        return 0;
    }

    PageTable &page_table = *reinterpret_cast<PageTable *>(page_directory_entry.PageFrameNumber * ARCH_PAGE_SIZE);

    int page_table_index = PAGE_TABLE_INDEX(virtual_address);
    PageTableEntry &page_table_entry = page_table.entries[page_table_index];

    if (!page_table_entry.Present)
    {
        return 0;
    }

    return (page_table_entry.PageFrameNumber * ARCH_PAGE_SIZE) + (virtual_address & 0xfff);
}

HjResult virtual_map(PageDirectory *page_directory, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags)
{
    ASSERT_INTERRUPTS_RETAINED();

    for (size_t i = 0; i < physical_range.size() / ARCH_PAGE_SIZE; i++)
    {
        size_t offset = i * ARCH_PAGE_SIZE;

        int page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address + offset);
        PageDirectoryEntry &page_directory_entry = page_directory->entries[page_directory_index];
        PageTable *page_table = reinterpret_cast<PageTable *>(page_directory_entry.PageFrameNumber * ARCH_PAGE_SIZE);

        if (!page_directory_entry.Present)
        {
            TRY(memory_alloc_identity(page_directory, MEMORY_CLEAR, (uintptr_t *)&page_table));

            page_directory_entry.Present = 1;
            page_directory_entry.Write = 1;
            page_directory_entry.User = 1;
            page_directory_entry.PageFrameNumber = (uint32_t)(page_table) >> 12;
        }

        int page_table_index = PAGE_TABLE_INDEX(virtual_address + offset);
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
    ASSERT_INTERRUPTS_RETAINED();

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
                assert(SUCCESS == virtual_map(page_directory, physical_range, virtual_address, flags));

                return {virtual_address, current_size};
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
    ASSERT_INTERRUPTS_RETAINED();

    for (size_t i = 0; i < virtual_range.size() / ARCH_PAGE_SIZE; i++)
    {
        size_t offset = i * ARCH_PAGE_SIZE;

        size_t page_directory_index = PAGE_DIRECTORY_INDEX(virtual_range.base() + offset);
        PageDirectoryEntry *page_directory_entry = &page_directory->entries[page_directory_index];

        if (!page_directory_entry->Present)
        {
            continue;
        }

        PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * ARCH_PAGE_SIZE);

        size_t page_table_index = PAGE_TABLE_INDEX(virtual_range.base() + offset);
        PageTableEntry *page_table_entry = &page_table->entries[page_table_index];

        if (page_table_entry->Present)
        {
            page_table_entry->as_uint = 0;
        }
    }

    paging_invalidate_tlb();
}

PageDirectory *page_directory_create()
{
    InterruptsRetainer retainer;

    PageDirectory *page_directory = nullptr;

    if (memory_alloc(kernel_page_directory(), sizeof(PageDirectory), MEMORY_CLEAR, (uintptr_t *)&page_directory) != SUCCESS)
    {
        Kernel::logln("Page directory allocation failed!");

        return nullptr;
    }

    memset(page_directory, 0, sizeof(PageDirectory));

    // Copy first gigs of virtual memory (kernel space);
    for (size_t i = 0; i < 256; i++)
    {
        PageDirectoryEntry *page_directory_entry = &page_directory->entries[i];

        page_directory_entry->User = 0;
        page_directory_entry->Write = 1;
        page_directory_entry->Present = 1;
        page_directory_entry->PageFrameNumber = (uint32_t)&_kernel_page_tables[i] / ARCH_PAGE_SIZE;
    }

    return page_directory;
}

void page_directory_destroy(PageDirectory *page_directory)
{
    InterruptsRetainer retainer;

    assert(page_directory != kernel_page_directory());

    for (size_t i = 256; i < 1024; i++)
    {
        PageDirectoryEntry *page_directory_entry = &page_directory->entries[i];

        if (page_directory_entry->Present)
        {
            PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * ARCH_PAGE_SIZE);

            for (size_t i = 0; i < 1024; i++)
            {
                PageTableEntry *page_table_entry = &page_table->entries[i];

                if (page_table_entry->Present)
                {
                    uintptr_t physical_address = page_table_entry->PageFrameNumber * ARCH_PAGE_SIZE;

                    MemoryRange physical_range{physical_address, ARCH_PAGE_SIZE};

                    physical_free(physical_range);
                }
            }

            memory_free(kernel_address_space(), (MemoryRange){(uintptr_t)page_table, sizeof(PageTable)});
        }
    }

    memory_free(kernel_address_space(), (MemoryRange){(uintptr_t)page_directory, sizeof(PageDirectory)});
}

void page_directory_switch(PageDirectory *page_directory)
{
    InterruptsRetainer retainer;
    paging_load_directory(virtual_to_physical(kernel_page_directory(), (uintptr_t)page_directory));
}

} // namespace Arch::x86_32
