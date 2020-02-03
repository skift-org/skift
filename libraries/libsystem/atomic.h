#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

void atomic_enable(void);
void atomic_disable(void);
void atomic_begin(void);
void atomic_end(void);
bool is_atomic(void);

#define ASSERT_ATOMIC assert(is_atomic())

#define ATOMIC(code)    \
    do                  \
    {                   \
        atomic_begin(); \
        code;           \
        atomic_end();   \
    } while (0);
