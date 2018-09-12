#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "types.h"
#include "utils.h"
#include "kernel/paging.h"

void memory_setup(uint used, uint total);

page_directorie_t * memory_kpdir();

uint memory_alloc(uint count);
void memory_free(uint addr, uint count);

page_directorie_t *memory_alloc_pdir();
void memory_free_pdir(page_directorie_t *pdir);

int memory_map(page_directorie_t *pdir, uint addr, uint count, int user);
int memory_unmap(page_directorie_t *pdir, uint addr, uint count);

int memory_identity_map(page_directorie_t *pdir, uint addr, uint count);
int memory_identity_unmap(page_directorie_t *pdir, uint addr, uint count);

void memory_dump(page_directorie_t * pdir);