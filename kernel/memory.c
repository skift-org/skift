/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* memory.c: Physical, virtual and logical memory managment                   */

#include <libmath/MinMax.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/runtime.h>

#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/paging.h"
#include "kernel/system.h"

/* --- Private functions ---------------------------------------------------- */

uint TOTAL_MEMORY = 0;
uint USED_MEMORY = 0;

uint FREE_MEMORY_SHORTCUT = 0;
uchar MEMORY[1024 * 1024 / 8] = {0};

#define PHYSICAL_IS_USED(__addr) \
    (MEMORY[(uint)(__addr) / PAGE_SIZE / 8] & (1 << ((uint)(__addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_USED(__addr) \
    (MEMORY[(uint)(__addr) / PAGE_SIZE / 8] |= (1 << ((uint)(__addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_FREE(__addr) \
    (MEMORY[(uint)(__addr) / PAGE_SIZE / 8] &= ~(1 << ((uint)(__addr) / PAGE_SIZE % 8)))

int physical_is_used(uint addr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        if (PHYSICAL_IS_USED(addr + (i * PAGE_SIZE)))
            return 1;
    }

    return 0;
}

void physical_set_used(uint addr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        if (!PHYSICAL_IS_USED(addr + (i * PAGE_SIZE)))
        {
            USED_MEMORY += PAGE_SIZE;
            PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
        }
    }
}

void physical_set_free(uint addr, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        if (PHYSICAL_IS_USED(addr + (i * PAGE_SIZE)))
        {
            USED_MEMORY -= PAGE_SIZE;
            PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
        }
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

    PANIC("Out of physical memory!\n\tTrying to allocat %d pages but free memory is %d pages !", count, (TOTAL_MEMORY - USED_MEMORY) / PAGE_SIZE);
    return 0;
}

void physical_free(uint addr, uint count)
{
    physical_set_free(addr, count);
}

/* --- Virtual memory managment --------------------------------------------- */

#define PD_INDEX(vaddr) ((vaddr) >> 22)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

PageDirectory kpdir __aligned(PAGE_SIZE) = {0};
PageTable kptable[256] __aligned(PAGE_SIZE) = {0};

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

    logger_error("Out of virtual memory!");
    return 0;
}

void virtual_free(PageDirectory *pdir, uint vaddr, uint count)
{
    // TODO: Check if the memory was allocated with ´virtual_alloc´.
    virtual_unmap(pdir, vaddr, count);
}

/* --- Public functions ----------------------------------------------------- */
bool is_memory_initialized = false;

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return MAX((uint)&__end, modules_get_end(minfo));
}

void memory_load_mmap(multiboot_info_t *mbootinfo)
{
    // Setup memory map
    TOTAL_MEMORY = 0;

    for (multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mbootinfo->mmap_addr; (u32)mmap < mbootinfo->mmap_addr + mbootinfo->mmap_length; mmap = (multiboot_memory_map_t *)((u32)mmap + mmap->size + sizeof(mmap->size)))
    {
        logger_info("Mmap: base_addr = 0x%x%08x, length = 0x%x%08x, type = 0x%x",
                    (u32)(mmap->addr >> 32),
                    (u32)(mmap->addr & 0xffffffff),
                    (u32)(mmap->len >> 32),
                    (u32)(mmap->len & 0xffffffff),
                    (u32)mmap->type);

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            TOTAL_MEMORY += mmap->len;
        }
        else
        {
            // Mark memory as used so we don't allocate there.
            for (u64 addr = mmap->addr; addr < mmap->len; addr += PAGE_SIZE)
            {
                PHYSICAL_SET_USED(addr);
            }
        }
    }
}

void memory_setup(multiboot_info_t *mbootinfo)
{
    memset(&MEMORY, 0, sizeof(MEMORY));

    // Setup the kernel pagedirectorie.
    for (uint i = 0; i < 256; i++)
    {
        PageDirectoryEntry *e = &kpdir.entries[i];
        e->User = 0;
        e->Write = 1;
        e->Present = 1;
        e->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
    }

    // Map the kernel memory
    memory_load_mmap(mbootinfo);
    memory_identity_map(&kpdir, 0, PAGE_ALIGN(get_kernel_end(mbootinfo)) / PAGE_SIZE + 1);
    virtual_unmap(memory_kpdir(), 0, 1); // Unmap the 0 page

    logger_info("%dKib of memory detected", TOTAL_MEMORY / 1024);
    logger_info("%dKib of memory are used by the kernel", USED_MEMORY / 1024);

    is_memory_initialized = true;

    paging_load_directorie(&kpdir);
    paging_enable();
}

PageDirectory *memory_kpdir()
{
    return &kpdir;
}

uint memory_alloc(PageDirectory *pdir, uint count, int user)
{
    if (count == 0)
        return 0;

    atomic_begin();

    uint paddr = physical_alloc(count);

    if (paddr == 0)
    {
        atomic_end();
        return 0;
    }

    uint vaddr = virtual_alloc(pdir, paddr, count, user);

    if (vaddr == 0)
    {
        physical_free(paddr, count);
        atomic_end();

        return 0;
    }

    atomic_end();

    memset((void *)vaddr, 0, count * PAGE_SIZE);

    return vaddr;
}

uint memory_alloc_at(PageDirectory *pdir, uint count, uint paddr, int user)
{
    if (count == 0)
        return 0;

    atomic_begin();

    uint vaddr = virtual_alloc(pdir, paddr, count, user);

    atomic_end();

    memset((void *)vaddr, 0, count * PAGE_SIZE);

    return vaddr;
}

// Alloc a identity mapped memory region, usefull for pagging data structurs
uint memory_alloc_identity(PageDirectory *pdir, uint count, int user)
{
    if (count == 0)
        return 0;

    atomic_begin();

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

                atomic_end();

                memset((void *)startaddr, 0, count * PAGE_SIZE);

                return startaddr;
            }
        }
        else
        {
            current_size = 0;
        }
    }

    atomic_end();

    logger_warn("alloc failed!");
    return 0;
}

void memory_free(PageDirectory *pdir, uint addr, uint count, int user)
{
    __unused(user);

    atomic_begin();

    if (virtual_present(pdir, addr, count))
    {
        physical_free(virtual2physical(pdir, addr), count);
        virtual_unmap(pdir, addr, count);
    }

    atomic_end();
}

// Alloc a pdir for a process
PageDirectory *memory_alloc_pdir()
{
    atomic_begin();

    PageDirectory *pdir = (PageDirectory *)memory_alloc_identity(&kpdir, 1, 0);

    if (pdir == NULL)
    {
        logger_error("Page directory allocation failled!");
        return NULL;
    }

    memset(pdir, 0, sizeof(PageDirectory));

    // Copy first gigs of virtual memory (kernel space);
    for (uint i = 0; i < 256; i++)
    {
        PageDirectoryEntry *e = &pdir->entries[i];
        e->User = 0;
        e->Write = 1;
        e->Present = 1;
        e->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
    }

    atomic_end();

    return pdir;
}

// Free the pdir of a dying process
void memory_free_pdir(PageDirectory *pdir)
{
    atomic_begin();

    for (size_t i = 256; i < 1024; i++)
    {
        PageDirectoryEntry *e = &pdir->entries[i];

        if (e->Present)
        {
            PageTable *pt = (PageTable *)(e->PageFrameNumber * PAGE_SIZE);

            for (size_t i = 0; i < 1024; i++)
            {
                PageTableEntry *p = &pt->entries[i];

                if (p->Present)
                {
                    physical_free(p->PageFrameNumber * PAGE_SIZE, 1);
                }
            }

            memory_free(&kpdir, (uint)pt, 1, 0);
        }
    }
    memory_free(&kpdir, (uint)pdir, 1, 0);

    atomic_end();
}

int memory_map(PageDirectory *pdir, uint addr, uint count, int user)
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

int memory_unmap(PageDirectory *pdir, uint addr, uint count)
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

int memory_identity_map(PageDirectory *pdir, uint addr, uint count)
{
    atomic_begin();
    physical_set_used(addr, count);
    virtual_map(pdir, addr, addr, count, 0);
    atomic_end();

    return 0;
}

int memory_identity_unmap(PageDirectory *pdir, uint addr, uint count)
{
    atomic_begin();
    physical_set_free(addr, count);
    virtual_unmap(pdir, addr, count);
    atomic_end();

    return 0;
}

void memory_dump(void)
{
    printf("\n\tMemory status:");
    printf("\n\t - Used  physical Memory: %12dkib", USED_MEMORY / 1024);
    printf("\n\t - Total physical Memory: %12dkib", TOTAL_MEMORY / 1024);
}

#define MEMORY_DUMP_REGION_START(__pdir, __addr)             \
    {                                                        \
        memory_used = true;                                  \
        memory_empty = false;                                \
        current_physical = virtual2physical(__pdir, __addr); \
        printf("%8x [%08x:", (__addr), current_physical);    \
    }

#define MEMORY_DUMP_REGION_END(__pdir, __addr)                            \
    {                                                                     \
        memory_used = false;                                              \
        printf("%08x] %08x", virtual2physical(__pdir, __addr), (__addr)); \
    }

void memory_layout_dump(PageDirectory *pdir, bool user)
{
    if (!is_memory_initialized)
        return;

    bool memory_used = false;
    bool memory_empty = true;
    uint current_physical = 0;

    for (int i = user ? 256 : 0; i < 1024; i++)
    {
        PageDirectoryEntry *pde = &pdir->entries[i];
        if (pde->Present)
        {
            PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);

            for (int j = 0; j < 1024; j++)
            {
                PageTableEntry *p = &ptable->entries[j];

                if (p->Present && !memory_used)
                {
                    MEMORY_DUMP_REGION_START(pdir, (i * 1024 + j) * PAGE_SIZE);
                }
                else if (!p->Present && memory_used)
                {
                    MEMORY_DUMP_REGION_END(pdir, (i * 1024 + j - 1) * PAGE_SIZE);
                }
                else if (p->Present)
                {
                    uint new_physical = virtual2physical(pdir, (i * 1024 + j) * PAGE_SIZE);

                    if (!(current_physical + PAGE_SIZE == new_physical))
                    {
                        printf("%08x | ", current_physical);
                        printf("%08x:", new_physical);
                    }

                    current_physical = new_physical;
                }
            }
        }
        else if (memory_used)
        {
            MEMORY_DUMP_REGION_END(pdir, (i * 1024 - 1) * PAGE_SIZE);
        }
    }

    if (memory_empty)
    {
        printf("[empty]");
    }
}

uint memory_get_used(void)
{
    uint result;

    atomic_begin();

    result = USED_MEMORY;

    atomic_end();

    return result;
}

uint memory_get_total(void)
{
    uint result;

    atomic_begin();

    result = TOTAL_MEMORY;

    atomic_end();

    return result;
}