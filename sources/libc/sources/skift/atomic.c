#include <skift/types.h>
#include <skift/atomic.h>

bool enabled = 0;
uint depth = 0;

void sk_atomic_enable()
{
    enabled = true;
}

void sk_atomic_disable()
{
    enabled = false;
}

void sk_atomic_begin()
{
    if (enabled)
    {
        asm volatile("cli");
        depth++;
    }
}

void sk_atomic_end()
{
    if (enabled)
    {
        depth--;
        if (depth == 0)
            asm volatile("sti");
    }
}