#pragma once

#include <abi/Memory.h>

#include "kernel/memory/MemoryRange.h"
#include "kernel/memory/Paging.h"

extern PageDirectory kpdir;
extern PageTable kptable[256];

bool virtual_present(PageDirectory *page_directory, uintptr_t virtual_address);

uintptr_t virtual_to_physical(PageDirectory *page_directory, uintptr_t virtual_address);

int virtual_map(PageDirectory *pdir, uint vaddr, uint paddr, uint count, bool user);

MemoryRange virtual_alloc(PageDirectory *pdir, MemoryRange physical_range, MemoryFlags flags);

void virtual_free(PageDirectory *page_directory, MemoryRange virtual_range);
