#pragma once

/* Copyright (c) 2018 Nicolas Van Bossuyt.                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "types.h"
#include "utils.h"
#include "kernel/paging.h"

void memory_setup(uint used, uint total);

uint memory_alloc(uint count);
void memory_free(uint addr, uint count);

int memory_map(page_directorie_t *pdir, uint addr, uint count);
int memory_unmap(page_directorie_t *pdir, uint addr, uint count);