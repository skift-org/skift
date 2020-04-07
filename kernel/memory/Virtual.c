/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/memory/Virtual.h"
#include "kernel/memory/Memory.h"
#include "kernel/system.h"

#define PD_INDEX(vaddr) ((vaddr) >> 22)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

PageDirectory kpdir __aligned(PAGE_SIZE) = {};
PageTable kptable[256] __aligned(PAGE_SIZE) = {};

int page_present(PageDirectory *pdir, uint vaddr)
{
    uint pdi = PD_INDEX(vaddr);
    uint pti = PT_INDEX(vaddr);

    PageDirectoryEntry *pde = &pdir->entries[pdi];

    if (!pde->Present)
    {
        return 0;
    }

    PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);
    PageTableEntry *p = &ptable->entries[pti];

    if (!p->Present)
    {
        return 0;
    }

    return 1;
}

int virtual_present(PageDirectory *pdir, uint vaddr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        if (!page_present(pdir, vaddr + offset))
        {
            return 0;
        }
    }

    return 1;
}

uint virtual2physical(PageDirectory *pdir, uint vaddr)
{
    uint pdi = PD_INDEX(vaddr);
    uint pti = PT_INDEX(vaddr);

    PageDirectoryEntry *pde = &pdir->entries[pdi];
    PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);
    PageTableEntry *p = &ptable->entries[pti];

    return (p->PageFrameNumber * PAGE_SIZE) + (vaddr & 0xfff);
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
            ptable = (PageTable *)memory_alloc_identity(pdir, 1, 0);

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

void virtual_unmap(PageDirectory *pdir, uint vaddr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);

        PageDirectoryEntry *pde = &pdir->entries[pdi];
        PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);
        PageTableEntry *p = &ptable->entries[pti];

        if (pde->Present)
            p->as_uint = 0;
    }

    paging_invalidate_tlb();
}

uint virtual_alloc(PageDirectory *pdir, uint paddr, uint count, int user)
{
    if (count == 0)
        return 0;

    uint current_size = 0;
    uint startaddr = 0;

    // we skip the first page to make null deref trigger a page fault
    for (size_t i = (user ? 256 : 1) * 1024; i < (user ? 1024 : 256) * 1024; i++)
    {
        int vaddr = i * PAGE_SIZE;

        if (!page_present(pdir, vaddr))
        {
            if (current_size == 0)
            {
                startaddr = vaddr;
            }

            current_size++;

            if (current_size == count)
            {
                virtual_map(pdir, startaddr, paddr, count, user);
                return startaddr;
            }
        }
        else
        {
            current_size = 0;
        }
    }

    PANIC("Out of virtual memory!");
}

void virtual_free(PageDirectory *pdir, uint vaddr, uint count)
{
    // TODO: Check if the memory was allocated with ´virtual_alloc´.
    virtual_unmap(pdir, vaddr, count);
}
