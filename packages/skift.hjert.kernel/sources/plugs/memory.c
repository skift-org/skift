#include <skift/__plugs__.h>
#include <skift/atomic.h>
#include "kernel/memory.h"

int __plug_memalloc_lock()
{
    sk_atomic_begin();
    return 0;
}

int __plug_memalloc_unlock()
{
    sk_atomic_end();
    return 0;
}

void *__plug_memalloc_alloc(uint size)
{
    void *p = (void *)memory_alloc(memory_kpdir(), size, 0);
    return p;
}

int __plug_memalloc_free(void *memory, uint size)
{
    memory_free(memory_kpdir(), (uint)memory, size, 0);
    return 0;
}