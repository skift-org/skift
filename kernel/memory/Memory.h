#pragma once

#include <abi/Memory.h>
#include <libsystem/Result.h>

#include "kernel/memory/MemoryRange.h"
#include "kernel/multiboot/Multiboot.h"

void memory_initialize(Multiboot *multiboot);

void memory_dump(void);

size_t memory_get_used(void);

size_t memory_get_total(void);

PageDirectory *memory_kpdir(void);

Result memory_map(PageDirectory *page_directory, MemoryRange range, MemoryFlags flags);

Result memory_map_identity(PageDirectory *page_directory, MemoryRange range, MemoryFlags flags);

Result memory_alloc(PageDirectory *page_directory, size_t size, MemoryFlags flags, uintptr_t *out_address);

Result memory_alloc_identity(PageDirectory *page_directory, MemoryFlags flags, uintptr_t *out_address);

Result memory_free(PageDirectory *page_directory, MemoryRange range);

PageDirectory *memory_pdir_create(void);

void memory_pdir_destroy(PageDirectory *pdir);

void memory_pdir_dump(PageDirectory *pdir, bool user);

void memory_pdir_switch(PageDirectory *pdir);
