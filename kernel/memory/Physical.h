#pragma once

#include <libsystem/Common.h>

extern uint TOTAL_MEMORY;
extern uint USED_MEMORY;
extern uchar MEMORY[1024 * 1024 / 8];

uint physical_alloc(uint count);

void physical_free(uint addr, uint count);

int physical_is_used(uint addr, uint count);

void physical_set_used(uint addr, uint caount);

void physical_set_free(uint addr, uint count);
