/* Copyright (c) 2018 Nicolas Van Bossuyt.                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/logger.h"
#include "kernel/physical.h"
#include "kernel/system.h"
#include "sync/atomic.h"

#define FRAME_SIZE 4096
#define FRAME_ALIGN(x) (x + FRAME_SIZE - (x % FRAME_SIZE))

uchar MEMORY[1024  * 1024 / 8];
uint MEMORY_TOTAL = 0;

void * get_free_frame()
{
    for(size_t i = 0; i < (MEMORY_TOTAL / FRAME_SIZE); i++)
    {
        void * p = (void *)(i * FRAME_SIZE);

        if (!physical_is_used(p))
        {
            return p;
        }
    }

    return NULL;
}

void physical_setup(uint memory)
{
    MEMORY_TOTAL = memory;
}

/* --- Allocations ---------------------------------------------------------- */

bool physical_is_used(void *mem)
{
    return MEMORY[(u32)mem / FRAME_SIZE / 8] & 1 << (u32)mem / FRAME_SIZE % 8;
}

void physical_used(void *mem)
{
    atomic_begin();
    MEMORY[(u32)mem / FRAME_SIZE / 8] |= 1 << (u32)mem / FRAME_SIZE % 8;
    atomic_end();
}

void physical_unused(void *mem)
{
    atomic_begin();
    MEMORY[(u32)mem / FRAME_SIZE / 8] &= ~(1 << (u32)mem / FRAME_SIZE % 8);
    atomic_end();
}

void *physical_alloc()
{
    atomic_begin();
    void * free_mem = get_free_frame();

    if (free_mem == NULL)
    {
        PANIC("Out of memory!");
    }

    physical_used(free_mem);

    atomic_end();

    return free_mem;
}

void *physical_alloc_contiguous(uint count)
{
    for(size_t i = 0; i < (MEMORY_TOTAL / FRAME_SIZE); i++)
    {
        
        bool is_valid = true;

        for(size_t j = 0; j < count; j++)
        {
            void * p = (void *)((i + j) * FRAME_SIZE);
            is_valid = is_valid && !physical_is_used(p);
        }
        
        if (is_valid)
        {
            for(size_t j = 0; j < count; j++)
            {
                void * p = (void *)((i + j) * FRAME_SIZE);
                physical_used(p);
            }

            return (void*)(i * FRAME_SIZE);
        }

    }

    log("physical_alloc_contiguous: failed!");
    return NULL;
}

void physical_free(void *mem)
{
    atomic_begin();
    if (physical_is_used(mem))
    {
        physical_unused(mem);
    }
    else
    {
        PANIC("Memory at 0x%x is already free!", mem);
    }
    atomic_end();
}

/* --- States --------------------------------------------------------------- */

uint physical_get_used()
{
    uint result = 0;

    for(size_t i = 0; i < (MEMORY_TOTAL / FRAME_SIZE); i++)
    {
        void * p = (void *)(i * FRAME_SIZE);

        if (physical_is_used(p))
        {
            result += FRAME_SIZE;
        }
    }

    return result;
}

uint physical_get_free()
{
    uint result = 0;

    for(size_t i = 0; i < (MEMORY_TOTAL / FRAME_SIZE); i++)
    {
        void * p = (void *)(i * FRAME_SIZE);

        if (!physical_is_used(p))
        {
            result += FRAME_SIZE;
        }
    }

    return result;
}

uint physical_get_total()
{
    return MEMORY_TOTAL;
}