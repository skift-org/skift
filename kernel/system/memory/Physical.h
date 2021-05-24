#pragma once

#include <libutils/Prelude.h>

#include "system/memory/MemoryRange.h"

extern size_t TOTAL_MEMORY;
extern size_t USED_MEMORY;
extern uint8_t MEMORY[1024 * 1024 / 8];

MemoryRange physical_alloc(size_t size);

void physical_free(MemoryRange range);

bool physical_is_used(MemoryRange range);

void physical_set_used(MemoryRange range);

void physical_set_free(MemoryRange range);
