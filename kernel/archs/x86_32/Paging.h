#pragma once

#include <libsystem/Common.h>

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

struct PACKED PageDirectory
{
    PageDirectoryEntry entries[PAGE_DIRECTORY_ENTRY_COUNT];
};

extern "C" void paging_enable();

extern "C" void paging_disable();

extern "C" void paging_load_directory(uintptr_t directory);

extern "C" void paging_invalidate_tlb();

void *kernel_address_space();

void virtual_initialize();

void virtual_memory_enable();

bool virtual_present(void *address_space, uintptr_t virtual_address);

uintptr_t virtual_to_physical(void *address_space, uintptr_t virtual_address);

Result virtual_map(void *address_space, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags);

MemoryRange virtual_alloc(void *address_space, MemoryRange physical_range, MemoryFlags flags);

void virtual_free(void *address_space, MemoryRange virtual_range);

void *address_space_create();

void address_space_destroy(void *address_space);

void address_space_switch(void *address_space);

} // namespace Arch::x86_32
