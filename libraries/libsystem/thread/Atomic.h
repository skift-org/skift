#pragma once

#include <libsystem/Common.h>

void atomic_enable();
void atomic_disable();
void atomic_begin();
void atomic_end();
bool is_atomic();

#define ASSERT_ATOMIC assert(is_atomic())

#define ATOMIC(code)    \
    do                  \
    {                   \
        atomic_begin(); \
        code;           \
        atomic_end();   \
    } while (0);
