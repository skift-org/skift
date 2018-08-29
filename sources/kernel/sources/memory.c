/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* memory.c: Physical, virtual and logical memory managment                   */

#include "types.h"
#include "utils.h"
#include "sync/atomic.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/paging.h"

/* --- Private functions ---------------------------------------------------- */

uint TOTAL_MEMORY = 0;
uint USED_MEMORY = 0;

page_directorie_t ALIGNED(kpdir, PAGE_SIZE);
page_table_t ALIGNED(kptable[256], PAGE_SIZE);

uchar MEMORY[1024 * 1024 / 8];

#define PHYSICAL_IS_USED(addr) \
    (MEMORY[(uint)(addr) / PAGE_SIZE / 8] & (1 << ((uint)(addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_USED(addr) \
    (MEMORY[(uint)(addr) / PAGE_SIZE / 8] |= (1 << ((uint)(addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_FREE(addr) \
    (MEMORY[(uint)(addr) / PAGE_SIZE / 8] &= ~(1 << ((uint)(addr) / PAGE_SIZE % 8)))

int physical_is_used(uint addr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
            return 1;
    }

    return 0;
}

void physical_set_used(uint addr, uint count)
{
    log("P_S_USED ADDR=%d COUNT=%d", addr, count);
    for (uint i = 0; i < count; i++)
    {
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
    }
}

void physical_set_free(uint addr, uint count)
{
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
    for (uint i = 0; i < count; i++)
    {
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
    }
}

uint physical_alloc(uint count)
{
    log("PALLOC COUNT=%d", count);
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
    {
        uint addr = i * PAGE_SIZE;
        if (!physical_is_used(addr, count))
        {
            physical_set_used(addr, count);
            return addr;
        }
    }

    return 0;
}

void physical_free(uint addr, uint count)
{
    physical_set_free(addr, count);
}

// Page directory index
#define PD_INDEX(vaddr) ((vaddr) >> 22)

// Page table index
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

int virtual_absent(page_directorie_t *pdir, uint vaddr, uint count)
{
    int absent = 1;

    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);

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

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);

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

int virtual_map(page_directorie_t *pdir, uint vaddr, uint paddr, uint count, bool user)
{

    log("VMAP VADDR=%d PADDR=%d COUNT=%d", vaddr, paddr, count);
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);

        page_directorie_entry_t *pde = &pdir->entries[pdi];
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);

        if (!pde->Present)
        {
            ptable = (page_table_t *)physical_alloc(1);
            virtual_map(&kpdir, (uint)ptable, (uint)ptable, 1, 0);

            pde->Present = 1;
            pde->Write = 1;
            pde->User = user;
            pde->PageFrameNumber = (u32)(ptable) >> 12;
        }

        page_t *p = &ptable->pages[pti];
        p->Present = 1;
        p->User = user;
        p->Write = 1;
        p->PageFrameNumber = (paddr + offset) >> 12;
    }

    paging_invalidate_tlb();

    return 0;
}

void virtual_unmap(page_directorie_t *pdir, uint vaddr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);

        page_directorie_entry_t *pde = &pdir->entries[pdi];
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
        page_t *p = &ptable->pages[pti];

        if (pde->Present)
            p->as_uint = 0;
    }

    paging_invalidate_tlb();
}

/* --- Public functions ----------------------------------------------------- */

void memory_setup(uint used, uint total)
{
    TOTAL_MEMORY = total;

    log("Memory: USED=%dko TOTAL=%dko", used / 1024, total / 1024);

    // Setup the kernel pagedirectorie.
    for (uint i = 0; i < 256; i++)
    {
        page_directorie_entry_t *e = &kpdir.entries[i];
        e->User = 0;
        e->Write = 1;
        e->Present = 1;
        e->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
    }

    // Map the kernel memory
    uint count = PAGE_ALIGN(used) / PAGE_SIZE;

    physical_set_used(0, count);
    virtual_map(&kpdir, 0, 0, count, 0);

    log("Enabling paging...");
    paging_load_directorie(&kpdir);
    paging_enable();
    log("Paging enabled!");
}

page_directorie_t *memory_kpdir()
{
    return &kpdir;
}

uint memory_alloc(uint count)
{
    atomic_begin();

    uint addr = physical_alloc(count);

    if (addr != 0)
        virtual_map(&kpdir, addr, addr, count, 0);

    atomic_end();

    return addr;
}

void memory_free(uint addr, uint count)
{
    atomic_begin();

    physical_free(addr, count);
    virtual_unmap(&kpdir, addr, count);

    atomic_end();
}

// Alloc a pdir for a process
page_directorie_t *memory_alloc_pdir()
{
    atomic_begin();

    page_directorie_t *pdir = (page_directorie_t *)memory_alloc(1);

    // Copy first gigs of virtual memory (kernel space);
    for (uint i = 0; i < 256; i++)
    {
        page_directorie_entry_t *e = &pdir->entries[i];
        e->User = 0;
        e->Write = 1;
        e->Present = 1;
        e->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
    }

    atomic_end();

    return pdir;
}

// Free the pdir of a dying process
void memory_free_pdir(page_directorie_t *pdir)
{
    atomic_begin();

    for (size_t i = 256; i < 1024; i++)
    {
        page_directorie_entry_t *e = &pdir->entries[i];

        if (e->Present)
        {
            page_table_t *pt = (page_table_t *)(e->PageFrameNumber * PAGE_SIZE);
            for (size_t i = 0; i < 1024; i++)
            {
                page_t *p = &pt->pages[i];

                if (p->Present)
                {
                    physical_free(p->PageFrameNumber * PAGE_SIZE, 1);
                }
            }

            memory_free((uint)pt, 1);
        }
    }

    memory_free((uint)pdir, 1);

    atomic_end();
}

int memory_map(page_directorie_t *pdir, uint addr, uint count, int user)
{
    atomic_begin();

    for (uint i = 0; i < count; i++)
    {
        uint vaddr = addr + i * PAGE_SIZE;

        if (!virtual_present(pdir, vaddr, 1))
        {
            uint paddr = physical_alloc(1);
            virtual_map(pdir, vaddr, paddr, 1, user);
        }
    }

    atomic_end();

    return 0;
}

int memory_unmap(page_directorie_t *pdir, uint addr, uint count)
{
    atomic_begin();

    for (uint i = 0; i < count; i++)
    {
        uint vaddr = addr + i * PAGE_SIZE;

        if (virtual_present(pdir, vaddr, 1))
        {
            physical_free(virtual2physical(pdir, vaddr), 1);
            virtual_unmap(pdir, vaddr, 1);
        }
    }

    atomic_end();

    return 0;
}
