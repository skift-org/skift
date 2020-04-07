#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/memory/MemoryRange.h"
#include "kernel/multiboot/Multiboot.h"

void memory_initialize(Multiboot *multiboot);

uint memory_alloc(PageDirectory *pdir, uint count, int user);

uint memory_alloc_at(PageDirectory *pdir, uint count, uint paddr, int user);

uint memory_alloc_identity(PageDirectory *pdir, uint count, int user);

void memory_free(PageDirectory *pdir, uint addr, uint count, int user);

PageDirectory *memory_alloc_pdir();

void memory_free_pdir(PageDirectory *pdir);

int memory_map(PageDirectory *pdir, uint addr, uint count, int user);

int memory_unmap(PageDirectory *pdir, uint addr, uint count);

void memory_identity_map_range(PageDirectory *pdir, MemoryRange range);

int memory_identity_map(PageDirectory *pdir, uint addr, uint count);

int memory_identity_unmap(PageDirectory *pdir, uint addr, uint count);

void memory_dump(void);

void memory_layout_dump(PageDirectory *pdir, bool user);

uint memory_get_used(void);

uint memory_get_total(void);

PageDirectory *memory_kpdir(void);