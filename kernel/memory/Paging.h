#pragma once

#include <libsystem/Common.h>

#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_DIRECTORY_ENTRY_COUNT 1024

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN(__x) ((__x) + PAGE_SIZE - ((__x) % PAGE_SIZE))

#define PAGE_ALIGN_UP(__x)  \
    ((__x % PAGE_SIZE == 0) \
         ? (__x)            \
         : (__x) + PAGE_SIZE - ((__x) % PAGE_SIZE))

#define PAGE_ALIGN_DOWN(__x) ((__x) - ((__x) % PAGE_SIZE))

#define IS_PAGE_ALIGN(__x) (__x % PAGE_SIZE == 0)

typedef union __packed // page table entry
{
    struct __packed
    {
        bool Present : 1;
        bool Write : 1;
        bool User : 1;
        bool PageLevelWriteThrough : 1;
        bool PageLevelCacheDisable : 1;
        bool Accessed : 1;
        bool Dirty : 1;
        bool Pat : 1;
        u32 Ignored : 4;
        u32 PageFrameNumber : 20;
    };

    u32 as_uint;
} PageTableEntry;

typedef struct __packed
{
    PageTableEntry entries[PAGE_TABLE_ENTRY_COUNT];
} PageTable;

typedef union __packed {
    struct __packed
    {
        bool Present : 1;
        bool Write : 1;
        bool User : 1;
        bool PageLevelWriteThrough : 1;
        bool PageLevelCacheDisable : 1;
        bool Accessed : 1;
        bool Ignored1 : 1;
        bool LargePage : 1;
        u32 Ignored2 : 4;
        u32 PageFrameNumber : 20;
    };
    u32 as_uint;
} PageDirectoryEntry;

typedef struct __packed
{
    PageDirectoryEntry entries[PAGE_DIRECTORY_ENTRY_COUNT];
} PageDirectory;

extern void paging_enable(void);
extern void paging_disable(void);
extern void paging_load_directory(uintptr_t directory);
extern void paging_invalidate_tlb(void);
