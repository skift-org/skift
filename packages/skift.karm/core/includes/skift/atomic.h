#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

void sk_atomic_enable();
void sk_atomic_disable();
void sk_atomic_begin();
void sk_atomic_end();

#define ASSERT_ATOMIC();

#define ATOMIC(code)       \
    do                     \
    {                      \
        sk_atomic_begin(); \
        code;              \
        sk_atomic_end();   \
    } while (0);
