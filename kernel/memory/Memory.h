#pragma once

#include "kernel/memory/MemoryRange.h"
#include "kernel/multiboot/Multiboot.h"

void memory_initialize(Multiboot *multiboot);

void memory_dump(void);

uint memory_get_used(void);

uint memory_get_total(void);

PageDirectory *memory_kpdir(void);

uint memory_alloc(PageDirectory *pdir, uint count, int user);

uint memory_alloc_identity(PageDirectory *pdir, uint count, int user);

void memory_free(PageDirectory *pdir, uint addr, uint count, int user);

int memory_map(PageDirectory *pdir, uint addr, uint count, int user);

int memory_map_eternal(PageDirectory *pdir, MemoryRange range);

int memory_unmap(PageDirectory *pdir, uint addr, uint count);

PageDirectory *memory_pdir_create(void);

void memory_pdir_destroy(PageDirectory *pdir);

void memory_pdir_dump(PageDirectory *pdir, bool user);

void memory_pdir_switch(PageDirectory *pdir);
