/* Copyright (c) 2018 Nicolas Van Bossuyt.                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/logging.h"
#include "kernel/physical.h"
#include "kernel/virtual.h"

uint virtual2physical(page_directorie_t *dir, uint virt)
{
    u32 pdindex = (u32)virt >> 22;
    u32 ptindex = (u32)virt >> 12 & 0x03FF;

    page_directorie_entry_t *pd_entry = &dir->entries[pdindex];

    if (pd_entry->Present)
    {
        page_table_t *pt = (page_table_t *)(pd_entry->PageFrameNumber * PAGE_SIZE);
        page_t *page = &pt->pages[ptindex];

        if (page->Present)
        {
            void *physical = (void *)((page->PageFrameNumber & ~0xfff) + ((u32)virt & 0xfff));
            return (uint)physical;
        }
    }

    return 0;
}

void virtual_map(page_directorie_t *dir, u32 virt, u32 phys, bool user)
{
    if (!IS_PAGE_ALIGN(phys) || !IS_PAGE_ALIGN(virt))
    {
        panic("Cannot page map 0x%x to 0x%x, because it's not page aligned!", virt, phys);
    }

    u32 pdindex = (u32)virt >> 22;
    u32 ptindex = (u32)virt >> 12 & 0x03FF;
    // debug("Mapping 0x%x(%d; %d) to 0x%x.", virtual, pdindex, ptindex, physical);

    page_directorie_entry_t *pd_entry = &dir->entries[pdindex];
    page_table_t *pt = (page_table_t *)(pd_entry->PageFrameNumber * PAGE_SIZE);

    if (!pd_entry->Present)
    {
        pt = paging_new_page_table();

        if (pt == NULL)
        {
            panic("Cannot allocate a new page table!");
        }

        pd_entry->Present = true;
        pd_entry->Write = true;
        pd_entry->User = user;
        pd_entry->PageFrameNumber = (u32)(pt) >> 12;
    }

    page_t *page = &pt->pages[ptindex];
    page->Present = true;
    page->User = user;
    page->Write = true;
    page->PageFrameNumber = phys >> 12;

    paging_invalidate_tlb();
}

void virtual_unmap(page_directorie_t *dir, u32 virt)
{
    u32 pdindex = (u32)virt >> 22;
    u32 ptindex = (u32)virt >> 12 & 0x03FF;

    page_directorie_entry_t *pd_entry = &dir->entries[pdindex];
    page_table_t *pt = (page_table_t *)(pd_entry->PageFrameNumber * PAGE_SIZE);
    page_t *page = &pt->pages[ptindex];

    page->as_uint = 0;
}