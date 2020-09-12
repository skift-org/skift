
#include <libsystem/thread/Atomic.h>

#include "arch/Arch.h"

static bool atomic_enabled = false;
static uint atomic_depth = 0;

bool is_atomic()
{
    return !atomic_enabled || atomic_depth > 0;
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
        arch_disable_interrupts();
        atomic_depth++;
    }
}

void atomic_end()
{
    if (atomic_enabled)
    {
        atomic_depth--;

        if (atomic_depth == 0)
            arch_enable_interrupts();
    }
}
