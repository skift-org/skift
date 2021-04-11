
#include <assert.h>
#include <libsystem/Logger.h>
#include <libsystem/io/Stream.h>
#include <string.h>

#include "archs/VirtualMemory.h"

#include "kernel/graphics/Graphics.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/memory/Memory.h"
#include "kernel/memory/MemoryObject.h"
#include "kernel/memory/Physical.h"

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

    for (size_t i = 0; i < handover->memory_map_size; i++)
    {
        MemoryMapEntry *entry = &handover->memory_map[i];

        if (entry->type == MEMORY_MAP_ENTRY_AVAILABLE)
        {
            physical_set_free(entry->range);
        }
    }

    arch_virtual_initialize();

    USED_MEMORY = 0;
    TOTAL_MEMORY = handover->memory_usable;

    logger_info("Mapping kernel...");
    memory_map_identity(arch_kernel_address_space(), kernel_memory_range(), MEMORY_NONE);

    logger_info("Mapping modules...");
    for (size_t i = 0; i < handover->modules_size; i++)
    {
        memory_map_identity(arch_kernel_address_space(), handover->modules[i].range, MEMORY_NONE);
    }

    // Unmap the 0 page
    MemoryRange page_zero{0, ARCH_PAGE_SIZE};
    arch_virtual_free(arch_kernel_address_space(), page_zero);
    physical_set_used(page_zero);

    arch_address_space_switch(arch_kernel_address_space());
    graphic_did_find_framebuffer(0, 0, 0, 0, 0);

    arch_virtual_memory_enable();

    logger_info("%uKio of memory detected", TOTAL_MEMORY / 1024);
    logger_info("%uKio of memory is used by the kernel", USED_MEMORY / 1024);

    logger_info("Paging enabled!");

    _memory_initialized = true;

    memory_object_initialize();
}

void memory_dump()
{
    stream_format(out_stream, "\n\tMemory status:");
    stream_format(out_stream, "\n\t - Used  physical Memory: %12dkib", USED_MEMORY / 1024);
    stream_format(out_stream, "\n\t - Total physical Memory: %12dkib", TOTAL_MEMORY / 1024);
}

size_t memory_get_used()
{
    InterruptsRetainer retainer;

    return USED_MEMORY;
}

size_t memory_get_total()
{
    InterruptsRetainer retainer;

    return TOTAL_MEMORY;
}

Result memory_map(void *address_space, MemoryRange virtual_range, MemoryFlags flags)
{
    assert(virtual_range.is_page_aligned());

    InterruptsRetainer retainer;

    for (size_t i = 0; i < virtual_range.size() / ARCH_PAGE_SIZE; i++)
    {
        uintptr_t virtual_address = virtual_range.base() + i * ARCH_PAGE_SIZE;

        if (!arch_virtual_present(address_space, virtual_address))
        {
            auto physical_range = physical_alloc(ARCH_PAGE_SIZE);
            Result virtual_map_result = arch_virtual_map(address_space, physical_range, virtual_address, flags);

            if (virtual_map_result != SUCCESS)
            {
                return virtual_map_result;
            }
        }
    }

    if (flags & MEMORY_CLEAR)
    {
        memset((void *)virtual_range.base(), 0, virtual_range.size());
    }

    return SUCCESS;
}

Result memory_map_identity(void *address_space, MemoryRange physical_range, MemoryFlags flags)
{
    assert(physical_range.is_page_aligned());

    InterruptsRetainer retainer;

    physical_set_used(physical_range);
    assert(SUCCESS == arch_virtual_map(address_space, physical_range, physical_range.base(), flags));

    if (flags & MEMORY_CLEAR)
    {
        memset((void *)physical_range.base(), 0, physical_range.size());
    }

    return SUCCESS;
}

Result memory_alloc(void *address_space, size_t size, MemoryFlags flags, uintptr_t *out_address)

{
    assert(IS_PAGE_ALIGN(size));

    InterruptsRetainer retainer;

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

    uintptr_t virtual_address = arch_virtual_alloc(address_space, physical_range, flags).base();

    if (!virtual_address)
    {
        physical_free(physical_range);

        logger_error("Failed to allocate memory: Not enough virtual memory!");
        return ERR_OUT_OF_MEMORY;
    }

    if (flags & MEMORY_CLEAR)
    {
        memset((void *)virtual_address, 0, size);
    }

    *out_address = virtual_address;
    return SUCCESS;
}

Result memory_alloc_identity(void *address_space, MemoryFlags flags, uintptr_t *out_address)
{
    InterruptsRetainer retainer;

    for (size_t i = 1; i < 256 * 1024; i++)
    {
        MemoryRange identity_range{i * ARCH_PAGE_SIZE, ARCH_PAGE_SIZE};

        if (!arch_virtual_present(address_space, identity_range.base()) &&
            !physical_is_used(identity_range))
        {
            physical_set_used(identity_range);
            assert(SUCCESS == arch_virtual_map(address_space, identity_range, identity_range.base(), flags));

            if (flags & MEMORY_CLEAR)
            {
                memset((void *)identity_range.base(), 0, ARCH_PAGE_SIZE);
            }

            *out_address = identity_range.base();

            return SUCCESS;
        }
    }

    logger_warn("Failed to allocate identity mapped page!");

    *out_address = 0;

    return ERR_OUT_OF_MEMORY;
}

Result memory_free(void *address_space, MemoryRange virtual_range)
{
    assert(virtual_range.is_page_aligned());

    InterruptsRetainer retainer;

    for (size_t i = 0; i < virtual_range.size() / ARCH_PAGE_SIZE; i++)
    {
        uintptr_t virtual_address = virtual_range.base() + i * ARCH_PAGE_SIZE;

        if (arch_virtual_present(address_space, virtual_address))
        {
            MemoryRange page_physical_range{arch_virtual_to_physical(address_space, virtual_address), ARCH_PAGE_SIZE};
            MemoryRange page_virtual_range{virtual_address, ARCH_PAGE_SIZE};

            physical_free(page_physical_range);
            arch_virtual_free(address_space, page_virtual_range);
        }
    }

    return SUCCESS;
}
