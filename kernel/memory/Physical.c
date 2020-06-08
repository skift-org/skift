
#include "kernel/memory/Physical.h"
#include "kernel/memory/Paging.h"
#include "kernel/system/System.h"

uint TOTAL_MEMORY = 0;
uint USED_MEMORY = 0;

uint FREE_MEMORY_SHORTCUT = 0;
uchar MEMORY[1024 * 1024 / 8] = {};

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

    system_panic("Out of physical memory!\n\tTrying to allocat %d pages but free memory is %d pages !", count, (TOTAL_MEMORY - USED_MEMORY) / PAGE_SIZE);
    return 0;
}

void physical_free(uint addr, uint count)
{
    physical_set_free(addr, count);
}
