/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include <skift/atomic.h>

static bool atomic_enabled = false;
static uint atomic_depth = 0;

bool sk_is_atomic()
{
    return !(atomic_enabled && atomic_depth == 0);
}

void sk_atomic_enable()
{
    atomic_enabled = true;
}

void sk_atomic_disable()
{
    atomic_enabled = false;
}

void sk_atomic_begin()
{
    if (atomic_enabled)
    {
        asm volatile("cli");
        atomic_depth++;
        
    }
}

void sk_atomic_end()
{
    if (atomic_enabled)
    {
        atomic_depth--;

        if (atomic_depth == 0)
        {
            asm volatile("sti");
        }
    }
}