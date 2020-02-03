/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>
#include <libsystem/atomic.h>

static bool atomic_enabled = false;
static uint atomic_depth = 0;

bool is_atomic()
{
    return !(atomic_enabled && atomic_depth == 0);
}

void atomic_enable()
{
    atomic_enabled = true;
}

void atomic_disable()
{
    atomic_enabled = false;
}

void atomic_begin()
{
    if (atomic_enabled)
    {
        asm volatile("cli");
        atomic_depth++;
    }
}

void atomic_end()
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