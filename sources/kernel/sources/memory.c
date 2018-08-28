/* Copyright (c) 2018 Nicolas Van Bossuyt.                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* memory.c: Physical, virtual and logical memory managment                   */

#include "types.h"
#include "utils.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/paging.h"

/* --- Private functions ---------------------------------------------------- */

uint TOTAL_MEMORY = 0;
uint USED_MEMORY = 0;

uchar MEMORY[1024 * 1024 / 8];

#define PHYSICAL_IS_USED(addr) \
    (MEMORY[(uint)addr / PAGE_SIZE / 8] & (1 << (uint)addr / PAGE_SIZE % 8))

#define PHYSICAL_SET_USED(addr) \
    MEMORY[(uint)addr / PAGE_SIZE / 8] |= (1 << (uint)addr / PAGE_SIZE % 8)

#define PHYSICAL_SET_FREE(addr) \
    MEMORY[(uint)addr / PAGE_SIZE / 8] &= ~(1 << (uint)addr / PAGE_SIZE % 8)

void physical_set_used(uint addr, uint count)
{
}

void physical_set_free(uint addr, uint count)
{
}

uint physical_alloc(uint count)
{
}

void physical_free(uint addr, uint count)
{
}

// Page directory index
#define PD_INDEX(vaddr) (vaddr >> 22)

// Page table index
#define PT_INDEX(vaddr) (vaddr >> 12 && 0x03ff)

int virtual_absent(page_directorie_t *pdir, uint vaddr, uint count)
{
    int absent = 1;

    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr);
        uint pti = PT_INDEX(vaddr);

        page_directorie_entry_t *pde = &pdir->entries[pdi];

        if (pde->Present)
        {
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
            page_t *p = &ptable->pages[pti];

            absent = absent && !p->Present;
        }
    }

    return absent;
}

int virtual_present(page_directorie_t *pdir, uint vaddr, uint count)
{
    int present = 1;

    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr);
        uint pti = PT_INDEX(vaddr);

        page_directorie_entry_t *pde = &pdir->entries[pdi];

        if (pde->Present)
        {
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
            page_t *p = &ptable->pages[pti];

            present = present && p->Present;
        }
    }

    return present;
}

uint virtual2physical(page_directorie_t *pdir, uint vaddr)
{
    uint pdi = PD_INDEX(vaddr);
    uint pti = PT_INDEX(vaddr);

    page_directorie_entry_t *pde = &pdir->entries[pdi];
    page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
    page_t *p = &ptable->pages[pti];

    return ((p->PageFrameNumber & ~0xfff) + (vaddr & 0xfff));
}

uint virtual_map(page_directorie_t *pdir, uint vaddr, uint count, uint paddr, bool user)
{
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);
    }

}

void virtual_unmap(page_directorie_t *pdir, uint vaddr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        if (virtual_present(pdir, vaddr + offset, 1))
        {
            uint pdi = PD_INDEX(vaddr + offset);
            uint pti = PT_INDEX(vaddr + offset);

            page_directorie_entry_t *pde = &pdir->entries[pdi];
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
            page_t *p = &ptable->pages[pti];

            p->as_uint = 0;
        }
    }
}

/* --- Public functions ----------------------------------------------------- */

page_directorie_t ALIGNED(kpdir, PAGE_SIZE);
page_table_t ALIGNED(kptable[256], PAGE_SIZE);

void memory_setup(uint used, uint total)
{
    TOTAL_MEMORY = total;
}

uint memory_alloc(uint count)
{
}

void memory_free(uint addr, uint count)
{
}

int memory_map(page_directorie_t *pdir, uint addr, uint count)
{
}

int memory_unmap(page_directorie_t *pdir, uint addr, uint count)
{
}
