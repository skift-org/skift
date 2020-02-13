#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#include <thirdparty/multiboot/Multiboot.h>

#include "kernel/paging.h"

extern uint TOTAL_MEMORY;

extern uint USED_MEMORY;

/* --- Physical Memory ------------------------------------------------------ */

uint physical_alloc(uint count);

void physical_free(uint addr, uint count);

void physical_set_used(uint addr, uint caount);

/* --- Virtual Memory ------------------------------------------------------- */

uint virtual_alloc(PageDirectory *pdir, uint paddr, uint count, int user);

void virtual_free(PageDirectory *pdir, uint vaddr, uint count);

/* --- Logical Memory ------------------------------------------------------- */

void memory_setup(multiboot_info_t *mbootinfo);

PageDirectory *memory_kpdir(void);

uint memory_alloc(PageDirectory *pdir, uint count, int user);

void memory_free(PageDirectory *pdir, uint addr, uint count, int user);

uint memory_alloc_at(PageDirectory *pdir, uint count, uint paddr, int user);

uint memory_alloc_identity(PageDirectory *pdir, uint count, int user);

PageDirectory *memory_alloc_pdir();

void memory_free_pdir(PageDirectory *pdir);

int memory_map(PageDirectory *pdir, uint addr, uint count, int user);

int memory_unmap(PageDirectory *pdir, uint addr, uint count);

int memory_identity_map(PageDirectory *pdir, uint addr, uint count);

int memory_identity_unmap(PageDirectory *pdir, uint addr, uint count);

void memory_dump(void);

void memory_layout_dump(PageDirectory *pdir, bool user);

uint memory_get_used(void);

uint memory_get_total(void);