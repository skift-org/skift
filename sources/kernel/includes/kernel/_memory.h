#pragma once
#include "types.h"
#include "utils.h"
#include "kernel/paging.h"

void memory_setup(uint kernel_end);

page_directorie_t * memory_construct_memory_space();
void memory_detroy_memory_space(page_directorie_t * page_dir);

void *memory_map(uint addr, size_t count);
void *memory_alloc(size_t count);
void memory_free(void *p, size_t count);
page_directorie_t * get_kernel_page_dir();