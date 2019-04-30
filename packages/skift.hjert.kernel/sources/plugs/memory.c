#include <skift/__plugs__.h>
#include <skift/atomic.h>
#include <skift/logger.h>
#include <skift/assert.h>
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
    sk_log(LOG_DEBUG, "Allocated %d pages for the kernel at %08x.", size, p);
    return p;
}

int __plug_memalloc_free(void *memory, uint size)
{
    memory_free(memory_kpdir(), (uint)memory, size, 0);
    sk_log(LOG_DEBUG, "Free'ed %d pages for the kernel at %08x.", size, memory);
    return 0;
}