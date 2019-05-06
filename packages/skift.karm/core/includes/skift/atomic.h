#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

void sk_atomic_enable(void);
void sk_atomic_disable(void);
void sk_atomic_begin(void);
void sk_atomic_end(void);
bool sk_is_atomic(void);

#define ASSERT_ATOMIC assert(sk_is_atomic())

#define ATOMIC(code)       \
    do                     \
    {                      \
        sk_atomic_begin(); \
        code;              \
        sk_atomic_end();   \
    } while (0);
