#pragma once

#include <libsystem/Common.h>

extern size_t TOTAL_MEMORY;
extern size_t USED_MEMORY;
extern uint8_t MEMORY[1024 * 1024 / 8];

uint physical_alloc(uint count);

void physical_free(uint addr, uint count);

int physical_is_used(uint addr, uint count);

void physical_set_used(uint addr, uint count);

void physical_set_free(uint addr, uint count);
