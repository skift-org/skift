#pragma once

#include <abi/Memory.h>
#include <libsystem/Result.h>

#include "arch/x86/Paging.h"
#include "kernel/memory/MemoryRange.h"

extern PageDirectory kpdir;
extern PageTable kptable[256];

bool virtual_present(PageDirectory *page_directory, uintptr_t virtual_address);

uintptr_t virtual_to_physical(PageDirectory *page_directory, uintptr_t virtual_address);

Result virtual_map(PageDirectory *page_directory, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags);

MemoryRange virtual_alloc(PageDirectory *page_directory, MemoryRange physical_range, MemoryFlags flags);

void virtual_free(PageDirectory *page_directory, MemoryRange virtual_range);
