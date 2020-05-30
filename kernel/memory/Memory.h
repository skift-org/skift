#pragma once

#include "kernel/memory/MemoryRange.h"
#include "kernel/multiboot/Multiboot.h"

#define MEMORY_NONE (0)
#define MEMORY_USER (1 << 0)
#define MEMORY_CLEAR (1 << 1)
typedef unsigned int MemoryFlags;

void memory_initialize(Multiboot *multiboot);

void memory_dump(void);

size_t memory_get_used(void);

size_t memory_get_total(void);

PageDirectory *memory_kpdir(void);

uintptr_t memory_alloc(PageDirectory *pdir, size_t size, MemoryFlags flags);

uintptr_t memory_alloc_identity_page(PageDirectory *pdir);

void memory_free(PageDirectory *pdir, uint addr, uint count, int user);

int memory_map(PageDirectory *pdir, uint addr, uint count, int user);

int memory_map_eternal(PageDirectory *pdir, MemoryRange range);

int memory_unmap(PageDirectory *pdir, uint addr, uint count);

PageDirectory *memory_pdir_create(void);

void memory_pdir_destroy(PageDirectory *pdir);

void memory_pdir_dump(PageDirectory *pdir, bool user);

void memory_pdir_switch(PageDirectory *pdir);
