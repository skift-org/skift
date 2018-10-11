#include "sync/atomic.h"
#include "utils/cpu.h"

bool is_atomic_enable = false;
uint ATOMIC_COUNTER = 0;

void atomic_enable()
{
    is_atomic_enable = true;
}

void atomic_disable()
{
    is_atomic_enable = false;
}

void atomic_begin()
{
    if (is_atomic_enable)
    {
        cli();
        ATOMIC_COUNTER++;
    }
}

void atomic_end()
{
    if (is_atomic_enable)
    {
        ATOMIC_COUNTER--;
        if (ATOMIC_COUNTER == 0)
            sti();
    }
}