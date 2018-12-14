/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* memory.c: Physical, virtual and logical memory managment                   */

#include <string.h>
#include <skift/types.h>
#include <skift/utils.h>
#include <skift/atomic.h>

#include "kernel/logger.h"
#include "kernel/paging.h"

#include "kernel/memory.h"

/* --- Private functions ---------------------------------------------------- */

uint TOTAL_MEMORY = 0;
uint USED_MEMORY = 0;

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
    for (uint i = 0; i < count; i++)
    {
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
    }
}

void physical_set_free(uint addr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
    }
}

uint physical_alloc(uint count)
{
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

/* --- Virtual memory managment --------------------------------------------- */

#define PD_INDEX(vaddr) ((vaddr) >> 22)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

page_directorie_t ALIGNED(kpdir, PAGE_SIZE);
page_table_t ALIGNED(kptable[256], PAGE_SIZE);

inline int page_present(page_directorie_t *pdir, uint vaddr)
{
    uint pdi = PD_INDEX(vaddr);
    uint pti = PT_INDEX(vaddr);

    page_directorie_entry_t *pde = &pdir->entries[pdi];

    if (!pde->Present)
    {
        return 0;
    }

    page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
    page_t *p = &ptable->pages[pti];

    if (!p->Present)
    {
        return 0;
    }

    return 1;
}

int virtual_present(page_directorie_t *pdir, uint vaddr, uint count)
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
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
        uint pti = PT_INDEX(vaddr + offset);

        page_directorie_entry_t *pde = &pdir->entries[pdi];
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);

        if (!pde->Present)
        {
            ptable = (page_table_t *)memory_alloc_identity(pdir, 1, 0);

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

uint virtual_alloc(page_directorie_t *pdir, uint paddr, uint count, int user)
{
    if (count == 0)
        return 0;

    uint current_size = 0;
    uint startaddr = 0;

    for (size_t i = (user ? 256 : 0) * 1024; i < (user ? 1024 : 256) * 1024; i++)
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

    return 0;
}

void virtual_free(page_directorie_t *pdir, uint vaddr, uint count)
{
    // TODO: Check if the memory was allocated with ´virtual_alloc´.
    virtual_unmap(pdir, vaddr, count);
}

/* --- Public functions ----------------------------------------------------- */

void memory_setup(uint used, uint total)
{
    TOTAL_MEMORY = total;

    memset(&MEMORY, 0, sizeof(MEMORY));

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
    memory_identity_map(&kpdir, 0, PAGE_ALIGN(used) / PAGE_SIZE);

    paging_load_directorie(&kpdir);
    paging_enable();
}

page_directorie_t *memory_kpdir()
{
    return &kpdir;
}

uint memory_alloc(page_directorie_t *pdir, uint count, int user)
{
    if (count == 0)
        return 0;

    sk_atomic_begin();

    uint paddr = physical_alloc(count);

    if (paddr == 0)
    {
        sk_atomic_end();

        // log("Failed!");
        return 0;
    }

    uint vaddr = virtual_alloc(pdir, paddr, count, user);

    if (vaddr == 0)
    {
        physical_free(paddr, count);
        sk_atomic_end();

        // log("Failed!");
        return 0;
    }

    sk_atomic_end();

    memset((void *)vaddr, 0, count * PAGE_SIZE);

    //log("PDIR=0x%x ADDR=0x%x COUNT=%d USER=%d", pdir, vaddr, count, user);

    return vaddr;
}

uint memory_alloc_at(page_directorie_t *pdir, uint count, uint paddr, int user)
{
    if (count == 0)
        return 0;

    sk_atomic_begin();

    uint vaddr = virtual_alloc(pdir, paddr, count, user);

    sk_atomic_end();

    memset((void *)vaddr, 0, count * PAGE_SIZE);

    // log("PDIR=0x%x ADDR=0x%x COUNT=%d USER=%d", pdir, vaddr, count, user);

    return vaddr;
}

// Alloc a identity mapped memory region, usefull for pagging data structurs
uint memory_alloc_identity(page_directorie_t *pdir, uint count, int user)
{
    if (count == 0)
        return 0;

    sk_atomic_begin();

    uint current_size = 0;
    uint startaddr = 0;

    for (size_t i = (user ? 256 : 0); i < (user ? 1024 : 256) * 1024; i++)
    {
        int addr = i * PAGE_SIZE;

        if (!(page_present(pdir, addr) || physical_is_used(addr, 1)))
        {
            if (current_size == 0)
            {
                startaddr = addr;
            }

            current_size++;

            if (current_size == count)
            {
                physical_set_used(startaddr, count);
                virtual_map(pdir, startaddr, startaddr, count, user);

                sk_atomic_end();

                log("PDIR=0x%x ADDR=0x%x COUNT=%d USER=%d", pdir, addr, count, user);

                return startaddr;
            }
        }
        else
        {
            current_size = 0;
        }
    }

    sk_atomic_end();

    //log("Failed!");

    return 0;
}

void memory_free(page_directorie_t *pdir, uint addr, uint count, int user)
{
    UNUSED(user);

    sk_atomic_begin();

    physical_free(addr, count);
    virtual_unmap(pdir, addr, count);

    sk_atomic_end();

    // log("PDIR=0x%x ADDR=0x%x COUNT=%d USER=%d", pdir, addr, count, user);
}

// Alloc a pdir for a process
page_directorie_t *memory_alloc_pdir()
{
    sk_atomic_begin();

    page_directorie_t *pdir = (page_directorie_t *)memory_alloc_identity(&kpdir, 1, 0);

    // Copy first gigs of virtual memory (kernel space);
    for (uint i = 0; i < 256; i++)
    {
        page_directorie_entry_t *e = &pdir->entries[i];
        e->User = 0;
        e->Write = 1;
        e->Present = 1;
        e->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
    }

    sk_atomic_end();

    // log("PDIR=0x%x", pdir);

    return pdir;
}

// Free the pdir of a dying process
void memory_free_pdir(page_directorie_t *pdir)
{
    sk_atomic_begin();

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

            memory_free(&kpdir, (uint)pt, 1, 0);
        }
    }
    memory_free(&kpdir, (uint)pdir, 1, 0);

    sk_atomic_end();

    // log("PDIR=0x%x", pdir);
}

int memory_map(page_directorie_t *pdir, uint addr, uint count, int user)
{
    sk_atomic_begin();

    for (uint i = 0; i < count; i++)
    {
        uint vaddr = addr + i * PAGE_SIZE;

        if (!virtual_present(pdir, vaddr, 1))
        {
            uint paddr = physical_alloc(1);
            virtual_map(pdir, vaddr, paddr, 1, user);
        }
    }

    sk_atomic_end();

    // log("PDIR=0x%x ADDR=0x%x COUNT=%d USER=%d", pdir, addr, count, user);

    return 0;
}

int memory_unmap(page_directorie_t *pdir, uint addr, uint count)
{
    sk_atomic_begin();

    for (uint i = 0; i < count; i++)
    {
        uint vaddr = addr + i * PAGE_SIZE;

        if (virtual_present(pdir, vaddr, 1))
        {
            physical_free(virtual2physical(pdir, vaddr), 1);
            virtual_unmap(pdir, vaddr, 1);
        }
    }

    sk_atomic_end();

    // log("PDIR=0x%x ADDR=0x%x COUNT=%d", pdir, addr, count);

    return 0;
}

int memory_identity_map(page_directorie_t *pdir, uint addr, uint count)
{
    physical_set_used(addr, count);
    virtual_map(pdir, addr, addr, count, 0);

    // log("PDIR=0x%x ADDR=0x%x COUNT=%d", pdir, addr, count);

    return 0;
}

int memory_identity_unmap(page_directorie_t *pdir, uint addr, uint count)
{
    physical_set_free(addr, count);
    virtual_unmap(pdir, addr, count);

    // log("PDIR=0x%x ADDR=0x%x COUNT=%d", pdir, addr, count);

    return 0;
}

void memory_dump(page_directorie_t *pdir)
{
    for (size_t i = 0; i < 1024; i++)
    {
        page_directorie_entry_t *pde = &pdir->entries[i];
        if (pde->Present)
        {
            printf("pdir[%d]={ PFN=%d PRESENT=%d WRITE=%d USER=%d }\n", i, pde->PageFrameNumber, pde->Present, pde->Write, pde->User);
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);

            for (size_t i = 0; i < 1024; i++)
            {
                page_t *p = &ptable->pages[i];
                if (p->Present)
                {
                    printf("ptable[%d]={ PFN=%d PRESENT=%d WRITE=%d USER=%d }\n", i, p->PageFrameNumber, p->Present, p->Write, p->User);
                }
            }
        }
    }
}
