
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/thread/Atomic.h>

#include "kernel/memory/Memory.h"
#include "kernel/memory/MemoryObject.h"
#include "kernel/memory/Physical.h"
#include "kernel/memory/Virtual.h"

static bool _memory_initialized = false;

extern int __start;
extern int __end;

static MemoryRange kernel_memory_range()
{
    return memory_range_around_non_aligned_address((uintptr_t)&__start, (size_t)&__end - (size_t)&__start);
}

void memory_initialize(Multiboot *multiboot)
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
        entry->PageFrameNumber = (size_t)&kptable[i] / PAGE_SIZE;
    }

    for (size_t i = 0; i < multiboot->memory_map_size; i++)
    {
        MemoryMapEntry *entry = &multiboot->memory_map[i];

        if (entry->type == MEMORY_MAP_ENTRY_AVAILABLE)
        {
            physical_set_free(entry->range.base, entry->range.size / PAGE_SIZE);
        }
    }

    USED_MEMORY = 0;
    TOTAL_MEMORY = multiboot->memory_usable;

    logger_info("Mapping kernel...");
    memory_map_identity(&kpdir, kernel_memory_range(), MEMORY_NONE);

    logger_info("Mapping modules...");
    for (size_t i = 0; i < multiboot->modules_size; i++)
    {
        memory_map_identity(&kpdir, multiboot->modules[i].range, MEMORY_NONE);
    }

    virtual_free(memory_kpdir(), (MemoryRange){0, PAGE_SIZE}); // Unmap the 0 page
    physical_set_used(0, 1);

    memory_pdir_switch(&kpdir);
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
    size_t result;

    atomic_begin();

    result = USED_MEMORY;

    atomic_end();

    return result;
}

size_t memory_get_total()
{
    size_t result;

    atomic_begin();

    result = TOTAL_MEMORY;

    atomic_end();

    return result;
}

PageDirectory *memory_kpdir()
{
    return &kpdir;
}

Result memory_map(PageDirectory *page_directory, MemoryRange virtual_range, MemoryFlags flags)
{
    assert(virtual_range.is_page_aligned());

    atomic_begin();

    for (size_t i = 0; i < virtual_range.size / PAGE_SIZE; i++)
    {
        uintptr_t virtual_address = virtual_range.base + i * PAGE_SIZE;

        if (!virtual_present(page_directory, virtual_address))
        {
            uintptr_t physical_address = physical_alloc(1);
            Result virtual_map_result = virtual_map(page_directory, MemoryRange{physical_address, PAGE_SIZE}, virtual_address, flags);

            if (virtual_map_result != SUCCESS)
            {
                return virtual_map_result;
            }
        }
    }

    atomic_end();

    if (flags & MEMORY_CLEAR)
        memset((void *)virtual_range.base, 0, virtual_range.size);

    return SUCCESS;
}

Result memory_map_identity(PageDirectory *page_directory, MemoryRange physical_range, MemoryFlags flags)
{
    assert(physical_range.is_page_aligned());

    size_t page_count = physical_range.size / PAGE_SIZE;

    atomic_begin();
    physical_set_used(physical_range.base, page_count);
    virtual_map(page_directory, physical_range, physical_range.base, flags);
    atomic_end();

    if (flags & MEMORY_CLEAR)
        memset((void *)physical_range.base, 0, physical_range.size);

    return SUCCESS;
}

Result memory_alloc(PageDirectory *page_directory, size_t size, MemoryFlags flags, uintptr_t *out_address)
{
    assert(IS_PAGE_ALIGN(size));

    if (!size)
    {
        *out_address = 0;
        logger_warn("Allocation with size=0!");
        return SUCCESS;
    }

    *out_address = 0;

    size_t page_count = size / PAGE_SIZE;

    atomic_begin();

    uintptr_t physical_address = physical_alloc(page_count);

    if (!physical_address)
    {
        atomic_end();

        logger_error("Failled to allocate memory: Not enough physical memory!");

        return ERR_OUT_OF_MEMORY;
    }

    uintptr_t virtual_address = virtual_alloc(page_directory, (MemoryRange){physical_address, size}, flags).base;

    if (!virtual_address)
    {
        physical_free(physical_address, page_count);
        atomic_end();

        logger_error("Failled to allocate memory: Not enough virtual memory!");

        return ERR_OUT_OF_MEMORY;
    }

    atomic_end();

    if (flags & MEMORY_CLEAR)
        memset((void *)virtual_address, 0, page_count * PAGE_SIZE);

    *out_address = virtual_address;
    return SUCCESS;
}

Result memory_alloc_identity(PageDirectory *page_directory, MemoryFlags flags, uintptr_t *out_address)
{
    atomic_begin();

    for (size_t i = 1; i < 256 * 1024; i++)
    {
        uintptr_t identity_address = i * PAGE_SIZE;

        if (!virtual_present(page_directory, identity_address) &&
            !physical_is_used(identity_address, 1))
        {
            physical_set_used(identity_address, 1);
            virtual_map(page_directory, MemoryRange{identity_address, PAGE_SIZE}, identity_address, flags);

            atomic_end();

            if (flags & MEMORY_CLEAR)
                memset((void *)identity_address, 0, PAGE_SIZE);

            *out_address = identity_address;

            return SUCCESS;
        }
    }

    atomic_end();

    logger_warn("Failled to allocate identity mapped page!");

    *out_address = 0;

    return ERR_OUT_OF_MEMORY;
}

Result memory_free(PageDirectory *page_directory, MemoryRange virtual_range)
{
    assert(virtual_range.is_page_aligned());

    atomic_begin();

    for (size_t i = 0; i < virtual_range.size / PAGE_SIZE; i++)
    {
        uintptr_t virtual_address = virtual_range.base + i * PAGE_SIZE;

        if (virtual_present(page_directory, virtual_address))
        {
            physical_free(virtual_to_physical(page_directory, virtual_address), 1);
            virtual_free(page_directory, (MemoryRange){virtual_address, PAGE_SIZE});
        }
    }

    atomic_end();

    return SUCCESS;
}

PageDirectory *memory_pdir_create()
{
    atomic_begin();

    PageDirectory *page_directory = nullptr;

    if (memory_alloc(&kpdir, sizeof(PageDirectory), MEMORY_CLEAR, (uintptr_t *)&page_directory) != SUCCESS)
    {
        logger_error("Page directory allocation failled!");
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
        page_directory_entry->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
    }

    atomic_end();

    return page_directory;
}

void memory_pdir_destroy(PageDirectory *page_directory)
{
    atomic_begin();

    for (size_t i = 256; i < 1024; i++)
    {
        PageDirectoryEntry *page_directory_entry = &page_directory->entries[i];

        if (page_directory_entry->Present)
        {
            PageTable *page_table = (PageTable *)(page_directory_entry->PageFrameNumber * PAGE_SIZE);

            for (size_t i = 0; i < 1024; i++)
            {
                PageTableEntry *page_table_entry = &page_table->entries[i];

                if (page_table_entry->Present)
                {
                    physical_free(page_table_entry->PageFrameNumber * PAGE_SIZE, 1);
                }
            }

            memory_free(&kpdir, (MemoryRange){(uintptr_t)page_table, sizeof(PageTable)});
        }
    }

    memory_free(&kpdir, (MemoryRange){(uintptr_t)page_directory, sizeof(PageDirectory)});

    atomic_end();
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
                    uint new_physical = virtual_to_physical(pdir, (i * 1024 + j) * PAGE_SIZE);

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

void memory_pdir_switch(PageDirectory *pdir)
{
    paging_load_directory(virtual_to_physical(&kpdir, (uintptr_t)pdir));
}
