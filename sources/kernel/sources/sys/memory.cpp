/* Copyright (c) 2018 Nicolas Van Bossuyt.                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/physical.h"
#include "kernel/virtual.h"

page_directorie_t ALIGNED(kernel_page_dir, PAGE_SIZE);
page_table_t ALIGNED(kernel_page_tables[256], PAGE_SIZE);

void memory_setup(uint kernel_end)
{
    for (size_t i = 0; i < 256; i++)
    {
        page_directorie_entry_t *entry = &kernel_page_dir.entries[i];
        entry->User = false;
        entry->Write = true;
        entry->Present = true;
        entry->PageFrameNumber = (uint)&kernel_page_tables[i] / PAGE_SIZE;
    }

    for (size_t i = 0; i < PAGE_ALIGN(kernel_end) / PAGE_SIZE; i++)
    {
        physical_used((void *)(i * PAGE_SIZE));
        virtual_map(&kernel_page_dir, (uint)(i * PAGE_SIZE), (uint)(i * PAGE_SIZE), false);
    }

    log("Memory: USED=%dk FREE=%dk TOTAL=%dk USAGE: %d%%",
    physical_get_used() / 1024,
    physical_get_free() / 1024,
    physical_get_total() / 1024,
    (physical_get_used() / 1024 * 100) / (physical_get_total() / 1024 * 100));

    paging_load_directorie(&kernel_page_dir);
    paging_enable();
    log("Paging is now enabled!");
}

void *memory_map(uint addr, size_t count)
{
    void *mem = physical_alloc_contiguous(count);

    for (size_t i = 0; i < count; i++)
    {
        uint virtual = addr + PAGE_SIZE * i;
        if (virtual2physical(&kernel_page_dir, virtual) == 0)
        {
            virtual_map(&kernel_page_dir, virtual, (uint)mem + PAGE_SIZE * i, false);
        }
    }

    return mem;
}

void *memory_alloc(size_t count)
{
    void *mem = physical_alloc_contiguous(count);

    for (size_t i = 0; i < count; i++)
    {
        virtual_map(&kernel_page_dir, (uint)mem + PAGE_SIZE * i, (uint)mem + PAGE_SIZE * i, false);
    }

    return mem;
}

void memory_free(void *p, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        physical_free(p + PAGE_SIZE * i);
        virtual_unmap(&kernel_page_dir, (uint)p + PAGE_SIZE * i);
    }
}

page_directorie_t *memory_construct_memory_space()
{
    page_directorie_t *dir = (page_directorie_t*)memory_alloc(1);
    memset(dir, 0, sizeof(page_directorie_t));

    for (size_t i = 0; i < 256; i++)
    {
        page_directorie_entry_t *entry = &kernel_page_dir.entries[i];
        entry->User = false;
        entry->Write = true;
        entry->Present = true;
        entry->PageFrameNumber = (uint)&kernel_page_tables[i] / PAGE_SIZE;
    }

    return dir;
}

void memory_detroy_memory_space(page_directorie_t *dir)
{
    UNUSED(dir);
}

page_directorie_t * get_kernel_page_dir()
{
    return &kernel_page_dir;
}
