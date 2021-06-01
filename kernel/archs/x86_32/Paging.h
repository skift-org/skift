#pragma once

#include <libutils/Prelude.h>

#include "archs/Memory.h"

namespace Arch::x86_32
{

#define PAGE_DIRECTORY_INDEX(vaddr) ((vaddr) >> 22)
#define PAGE_TABLE_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_DIRECTORY_ENTRY_COUNT 1024

union PACKED PageTableEntry
{
    struct PACKED
    {
        bool Present : 1;
        bool Write : 1;
        bool User : 1;
        bool PageLevelWriteThrough : 1;
        bool PageLevelCacheDisable : 1;
        bool Accessed : 1;
        bool Dirty : 1;
        bool Pat : 1;
        uint32_t Ignored : 4;
        uint32_t PageFrameNumber : 20;
    };

    uint32_t as_uint;
};

struct PACKED PageTable
{
    PageTableEntry entries[PAGE_TABLE_ENTRY_COUNT];
};

union PACKED PageDirectoryEntry
{
    struct PACKED
    {
        bool Present : 1;
        bool Write : 1;
        bool User : 1;
        bool PageLevelWriteThrough : 1;
        bool PageLevelCacheDisable : 1;
        bool Accessed : 1;
        bool Ignored1 : 1;
        bool LargePage : 1;
        uint32_t Ignored2 : 4;
        uint32_t PageFrameNumber : 20;
    };
    uint32_t as_uint;
};

struct PACKED PageDirectory : public Arch::AddressSpace
{
    PageDirectoryEntry entries[PAGE_DIRECTORY_ENTRY_COUNT];
};

extern "C" void paging_enable();

extern "C" void paging_disable();

extern "C" void paging_load_directory(uintptr_t directory);

extern "C" void paging_invalidate_tlb();

PageDirectory *kernel_page_directory();

void virtual_initialize();

void virtual_memory_enable();

bool virtual_present(PageDirectory *page_directory, uintptr_t virtual_address);

uintptr_t virtual_to_physical(PageDirectory *page_directory, uintptr_t virtual_address);

HjResult virtual_map(PageDirectory *page_directory, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags);

MemoryRange virtual_alloc(PageDirectory *page_directory, MemoryRange physical_range, MemoryFlags flags);

void virtual_free(PageDirectory *page_directory, MemoryRange virtual_range);

PageDirectory *page_directory_create();

void page_directory_destroy(PageDirectory *page_directory);

void page_directory_switch(PageDirectory *page_directory);

} // namespace Arch::x86_32
