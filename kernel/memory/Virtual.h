#pragma once

#include "kernel/memory/Paging.h"

extern PageDirectory kpdir;
extern PageTable kptable[256];

bool virtual_present(PageDirectory *page_directory, uintptr_t virtual_address);

uintptr_t virtual_to_physical(PageDirectory *page_directory, uintptr_t virtual_address);

int virtual_map(PageDirectory *pdir, uint vaddr, uint paddr, uint count, bool user);

uint virtual_alloc(PageDirectory *pdir, uint paddr, uint count, int user);

void virtual_free(PageDirectory *pdir, uint vaddr, uint count);
