#pragma once
#include "types.h"

void physical_setup(uint memory);

bool physical_is_used(void *mem);
void physical_used(void *mem);
void physical_unused(void *mem);

void *physical_alloc();
void *physical_alloc_contiguous(uint count);
void physical_free(void *mem);

uint physical_get_used();
uint physical_get_free();
uint physical_get_total();