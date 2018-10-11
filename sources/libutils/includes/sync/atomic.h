#pragma once
#include "types.h"

#define ATOMIC(code) do { code; } while(0);

void atomic_enable();
void atomic_disable();
void atomic_begin();
void atomic_end();