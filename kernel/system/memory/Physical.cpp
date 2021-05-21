#include "system/Streams.h"

#include "archs/Memory.h"

#include "system/interrupts/Interupts.h"
#include "system/memory/Physical.h"
#include "system/system/System.h"

size_t TOTAL_MEMORY = 0;
size_t USED_MEMORY = 0;

size_t best_bet = 0;
uint8_t MEMORY[1024 * 1024 / 8] = {};

bool physical_page_is_used(uintptr_t address)
{
    uintptr_t page = address / ARCH_PAGE_SIZE;

    return MEMORY[page / 8] & (1 << (page % 8));
}

void physical_page_set_used(uintptr_t address)
{
    uintptr_t page = address / ARCH_PAGE_SIZE;

    if (page == best_bet)
    {
        best_bet++;
    }

    MEMORY[page / 8] |= 1 << (page % 8);
}

void physical_page_set_free(uintptr_t address)
{
    uintptr_t page = address / ARCH_PAGE_SIZE;

    if (page < best_bet)
    {
        best_bet = page;
    }

    MEMORY[page / 8] &= ~(1 << (page % 8));
}

MemoryRange physical_alloc(size_t size)
{
    ASSERT_INTERRUPTS_RETAINED();

    assert(IS_PAGE_ALIGN(size));

    for (size_t i = best_bet; i < ((TOTAL_MEMORY - size) / ARCH_PAGE_SIZE); i++)
    {
        MemoryRange range(i * ARCH_PAGE_SIZE, size);

        if (!physical_is_used(range))
        {
            physical_set_used(range);
            return range;
        }
    }

    system_panic("Out of physical memory!\tTrying to allocat %dkio but free memory is %dkio !", size / 1024, (TOTAL_MEMORY - USED_MEMORY) / 1024);
}

void physical_free(MemoryRange range)
{
    ASSERT_INTERRUPTS_RETAINED();

    assert(range.is_page_aligned());

    physical_set_free(range);
}

bool physical_is_used(MemoryRange range)
{
    ASSERT_INTERRUPTS_RETAINED();

    assert(range.is_page_aligned());

    for (size_t i = 0; i < range.page_count(); i++)
    {
        uintptr_t address = range.base() + i * ARCH_PAGE_SIZE;

        if (physical_page_is_used(address))
        {
            return true;
        }
    }

    return false;
}

void physical_set_used(MemoryRange range)
{
    ASSERT_INTERRUPTS_RETAINED();

    assert(range.is_page_aligned());

    for (size_t i = 0; i < range.page_count(); i++)
    {
        uintptr_t address = range.base() + i * ARCH_PAGE_SIZE;

        if (!physical_page_is_used(address))
        {
            USED_MEMORY += ARCH_PAGE_SIZE;
            physical_page_set_used(address);
        }
    }
}

void physical_set_free(MemoryRange range)
{
    ASSERT_INTERRUPTS_RETAINED();

    assert(range.is_page_aligned());

    for (size_t i = 0; i < range.page_count(); i++)
    {
        uintptr_t address = range.base() + i * ARCH_PAGE_SIZE;

        if (physical_page_is_used(address))
        {
            USED_MEMORY -= ARCH_PAGE_SIZE;
            physical_page_set_free(address);
        }
    }
}
