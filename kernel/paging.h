#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_DIRECTORIE_ENTRY_COUNT 1024

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN(__x) ((__x) + PAGE_SIZE - ((__x) % PAGE_SIZE))

#define PAGE_ALIGN_UP(__x) ((__x) + PAGE_SIZE - ((__x) % PAGE_SIZE))
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
} page_t;

typedef struct __packed
{
    page_t pages[PAGE_TABLE_ENTRY_COUNT];
} page_table_t;

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
} page_directorie_entry_t;

typedef struct __packed
{
    page_directorie_entry_t entries[PAGE_DIRECTORIE_ENTRY_COUNT];
} page_directorie_t;

extern void paging_enable(void);
extern void paging_disable(void);
extern void paging_load_directorie(page_directorie_t *directorie);
extern void paging_invalidate_tlb();