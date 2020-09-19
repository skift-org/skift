
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/thread/Atomic.h>

#include "kernel/memory/Memory.h"
#include "kernel/memory/MemoryObject.h"
#include "kernel/memory/Physical.h"
#include "kernel/memory/Virtual.h"
#include "kernel/graphics/Graphics.h"

static bool _memory_initialized = false;

extern int __start;
extern int __end;

static MemoryRange kernel_memory_range()
{
    return MemoryRange::around_non_aligned_address((uintptr_t)&__start, (size_t)&__end - (size_t)&__start);
}

void memory_initialize(Handover *handover)
{
    logger_info("Initializing memory management...");

    for (size_t i = 0; i < 1024 * 1024 / 8; i++)
    {
        MEMORY[i] = 0xff;
    }

    // Setup the kernel pagedirectory.
    for (size_t i = 0; i < 256; i++)
    {
        PageDirectoryEntry *entry = &kpdir.entries[i];
        entry->User = 0;
        entry->Write = 1;
        entry->Present = 1;
        entry->PageFrameNumber = (size_t)&kptable[i] / ARCH_PAGE_SIZE;
    }

    for (size_t i = 0; i < handover->memory_map_size; i++)
    {
        MemoryMapEntry *entry = &handover->memory_map[i];

        if (entry->type == MEMORY_MAP_ENTRY_AVAILABLE)
        {
            physical_set_free(entry->range);
        }
    }

    USED_MEMORY = 0;
    TOTAL_MEMORY = handover->memory_usable;

    logger_info("Mapping kernel...");
    memory_map_identity(&kpdir, kernel_memory_range(), MEMORY_NONE);

    logger_info("Mapping modules...");
    for (size_t i = 0; i < handover->modules_size; i++)
    {
        memory_map_identity(&kpdir, handover->modules[i].range, MEMORY_NONE);
    }

    // Unmap the 0 page
    MemoryRange page_zero{0, ARCH_PAGE_SIZE};
    virtual_free(memory_kpdir(), page_zero);
    physical_set_used(page_zero);

    memory_pdir_switch(&kpdir);

    graphic_did_find_framebuffer(0, 0, 0);
    paging_enable();

    logger_info("%uKio of memory detected", TOTAL_MEMORY / 1024);
    logger_info("%uKio of memory is used by the kernel", USED_MEMORY / 1024);

    logger_info("Paging enabled!");

    _memory_initialized = true;

    memory_object_initialize();
}

void memory_dump()
{
    printf("\n\tMemory status:");
    printf("\n\t - Used  physical Memory: %12dkib", USED_MEMORY / 1024);
    printf("\n\t - Total physical Memory: %12dkib", TOTAL_MEMORY / 1024);
}

size_t memory_get_used()
{
    AtomicHolder holder;

    return USED_MEMORY;
}

size_t memory_get_total()
{
    AtomicHolder holder;

    return TOTAL_MEMORY;
}

PageDirectory *memory_kpdir()
{
    return &kpdir;
}

Result memory_map(PageDirectory *page_directory, MemoryRange virtual_range, MemoryFlags flags)
{
    assert(virtual_range.is_page_aligned());

    AtomicHolder holder;

    for (size_t i = 0; i < virtual_range.size() / ARCH_PAGE_SIZE; i++)
    {
        uintptr_t virtual_address = virtual_range.base() + i * ARCH_PAGE_SIZE;

        if (!virtual_present(page_directory, virtual_address))
        {
            auto physical_range = physical_alloc(ARCH_PAGE_SIZE);
            Result virtual_map_result = virtual_map(page_directory, physical_range, virtual_address, flags);

            if (virtual_map_result != SUCCESS)
            {
                return virtual_map_result;
            }
        }
    }

    if (flags & MEMORY_CLEAR)
        memset((void *)virtual_range.base(), 0, virtual_range.size());

    return SUCCESS;
}

Result memory_map_identity(PageDirectory *page_directory, MemoryRange physical_range, MemoryFlags flags)
{
    assert(physical_range.is_page_aligned());

    AtomicHolder holder;

    physical_set_used(physical_range);
    virtual_map(page_directory, physical_range, physical_range.base(), flags);

    if (flags & MEMORY_CLEAR)
        memset((void *)physical_range.base(), 0, physical_range.size());

    return SUCCESS;
}

Result memory_alloc(PageDirectory *page_directory, size_t size, MemoryFlags flags, uintptr_t *out_address)
{
    assert(IS_PAGE_ALIGN(size));

    AtomicHolder holder;

    if (!size)
    {
        *out_address = 0;
        logger_warn("Allocation with size=0!");
        return SUCCESS;
    }

    *out_address = 0;

    auto physical_range = physical_alloc(size);

    if (physical_range.empty())
    {
        logger_error("Failed to allocate memory: Not enough physical memory!");
        return ERR_OUT_OF_MEMORY;
    }

    uintptr_t virtual_address = virtual_alloc(page_directory, physical_range, flags).base();

    if (!virtual_address)
    {
        physical_free(physical_range);

        logger_error("Failed to allocate memory: Not enough virtual memory!");
        return ERR_OUT_OF_MEMORY;
    }

    if (flags & MEMORY_CLEAR)
        memset((void *)virtual_address, 0, size);

    *out_address = virtual_address;
    return SUCCESS;
}

Result memory_alloc_identity(PageDirectory *page_directory, MemoryFlags flags, uintptr_t *out_address)
{
    AtomicHolder holder;

    for (size_t i = 1; i < 256 * 1024; i++)
    {
        MemoryRange identity_range{i * ARCH_PAGE_SIZE, ARCH_PAGE_SIZE};

        if (!virtual_present(page_directory, identity_range.base()) &&
            !physical_is_used(identity_range))
        {
            physical_set_used(identity_range);
            virtual_map(page_directory, identity_range, identity_range.base(), flags);

            if (flags & MEMORY_CLEAR)
                memset((void *)identity_range.base(), 0, ARCH_PAGE_SIZE);

            *out_address = identity_range.base();

            return SUCCESS;
        }
    }

    logger_warn("Failed to allocate identity mapped page!");

    *out_address = 0;

    return ERR_OUT_OF_MEMORY;
}

Result memory_free(PageDirectory *page_directory, MemoryRange virtual_range)
{
    assert(virtual_range.is_page_aligned());

    AtomicHolder holder;

    for (size_t i = 0; i < virtual_range.size() / ARCH_PAGE_SIZE; i++)
    {
        uintptr_t virtual_address = virtual_range.base() + i * ARCH_PAGE_SIZE;

        if (virtual_present(page_directory, virtual_address))
        {
            MemoryRange page_physical_range{virtual_to_physical(page_directory, virtual_address), ARCH_PAGE_SIZE};
            MemoryRange page_virtual_range{virtual_address, ARCH_PAGE_SIZE};

            physical_free(page_physical_range);
            virtual_free(page_directory, page_virtual_range);
        }
    }

    return SUCCESS;
}

PageDirectory *memory_pdir_create()
{
    AtomicHolder holder;

    PageDirectory *page_directory = nullptr;

    if (memory_alloc(&kpdir, sizeof(PageDirectory), MEMORY_CLEAR, (uintptr_t *)&page_directory) != SUCCESS)
    {
        logger_error("Page directory allocation failed!");

        return nullptr;
    }

    memset(page_directory, 0, sizeof(PageDirectory));

    // Copy first gigs of virtual memory (kernel space);
    for (uint i = 0; i < 256; i++)
    {
        PageDirectoryEntry *page_directory_entry = &page_directory->entries[i];

        page_directory_entry->User = 0;
        page_directory_entry->Write = 1;
        page_directory_entry->Present = 1;
        page_directory_entry->PageFrameNumber = (uint)&kptable[i] / ARCH_PAGE_SIZE;
    }

    return page_directory;
}

void memory_pdir_destroy(PageDirectory *page_directory)
{
    AtomicHolder holder;

    for (size_t i = 256; i < 1024; i++)
    {
        PageDirectoryEntry *page_directory_entry = &page_directory->entries[i];

        if (page_directory_entry->Present)
        {
            PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * ARCH_PAGE_SIZE);

            for (size_t i = 0; i < 1024; i++)
            {
                PageTableEntry *page_table_entry = &page_table->entries[i];

                if (page_table_entry->Present)
                {
                    uintptr_t physical_address = page_table_entry->PageFrameNumber * ARCH_PAGE_SIZE;

                    MemoryRange physical_range{physical_address, ARCH_PAGE_SIZE};

                    physical_free(physical_range);
                }
            }

            memory_free(&kpdir, (MemoryRange){(uintptr_t)page_table, sizeof(PageTable)});
        }
    }

    memory_free(&kpdir, (MemoryRange){(uintptr_t)page_directory, sizeof(PageDirectory)});
}

#define MEMORY_DUMP_REGION_START(__pdir, __addr)                \
    {                                                           \
        memory_used = true;                                     \
        memory_empty = false;                                   \
        current_physical = virtual_to_physical(__pdir, __addr); \
        printf("\n\t %8x [%08x:", (__addr), current_physical);  \
    }

#define MEMORY_DUMP_REGION_END(__pdir, __addr)                               \
    {                                                                        \
        memory_used = false;                                                 \
        printf("%08x] %08x", virtual_to_physical(__pdir, __addr), (__addr)); \
    }

void memory_pdir_dump(PageDirectory *pdir, bool user)
{
    if (!_memory_initialized)
        return;

    bool memory_used = false;
    bool memory_empty = true;
    uint current_physical = 0;

    for (int i = user ? 256 : 0; i < 1024; i++)
    {
        PageDirectoryEntry *pde = &pdir->entries[i];
        if (pde->Present)
        {
            PageTable *ptable = (PageTable *)(pde->PageFrameNumber * ARCH_PAGE_SIZE);

            for (int j = 0; j < 1024; j++)
            {
                PageTableEntry *p = &ptable->entries[j];

                if (p->Present && !memory_used)
                {
                    MEMORY_DUMP_REGION_START(pdir, (i * 1024 + j) * ARCH_PAGE_SIZE);
                }
                else if (!p->Present && memory_used)
                {
                    MEMORY_DUMP_REGION_END(pdir, (i * 1024 + j - 1) * ARCH_PAGE_SIZE);
                }
                else if (p->Present)
                {
                    uint new_physical = virtual_to_physical(pdir, (i * 1024 + j) * ARCH_PAGE_SIZE);

                    if (!(current_physical + ARCH_PAGE_SIZE == new_physical))
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
            MEMORY_DUMP_REGION_END(pdir, (i * 1024 - 1) * ARCH_PAGE_SIZE);
        }
    }

    if (memory_empty)
    {
        printf("[empty]");
    }
}

void memory_pdir_switch(PageDirectory *pdir)
{
    AtomicHolder holder;
    paging_load_directory(virtual_to_physical(&kpdir, (uintptr_t)pdir));
}
