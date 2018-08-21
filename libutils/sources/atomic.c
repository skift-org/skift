#include "sync/atomic.h"
#include "utils/cpu.h"

bool is_atomic_enable = false;
uint ATOMIC_COUNTER = 0;

void atomic_enable()
{
    is_atomic_enable = true;
}

void atomic_begin()
{
    if (is_atomic_enable)
        cli();
        ATOMIC_COUNTER++;
}

void atomic_end()
{
    ATOMIC_COUNTER--;
    if (is_atomic_enable && ATOMIC_COUNTER == 0)
        sti();
}