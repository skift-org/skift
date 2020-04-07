#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/memory/Paging.h"

extern PageDirectory kpdir;
extern PageTable kptable[256];

int page_present(PageDirectory *pdir, uint vaddr);

int virtual_present(PageDirectory *pdir, uint vaddr, uint count);

uint virtual2physical(PageDirectory *pdir, uint vaddr);

int virtual_map(PageDirectory *pdir, uint vaddr, uint paddr, uint count, bool user);

void virtual_unmap(PageDirectory *pdir, uint vaddr, uint count);

uint virtual_alloc(PageDirectory *pdir, uint paddr, uint count, int user);

void virtual_free(PageDirectory *pdir, uint vaddr, uint count);
