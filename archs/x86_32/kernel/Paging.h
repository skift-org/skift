#pragma once

#include <libsystem/Common.h>

#include "archs/Memory.h"

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
