// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include "liballoc.h"
#include "sync/atomic.h"
#include "kernel/memory.h"
#include "kernel/logger.h"

int liballoc_lock()
{
    atomic_begin();
    return 0;
}

int liballoc_unlock()
{
    atomic_end();
    return 0;
}

void* liballoc_alloc(size_t size)
{
    void* p = (void*)memory_alloc(memory_kpdir(), size, 0);
    log("LIBALLOC alloc size: %d at 0x%x.", size, p);
    return p;
}

int liballoc_free(void* p,size_t size)
{
    memory_free(memory_kpdir(),(uint)p, size, 0);
    log("LIBALLOC free: %d at 0x%x.", size, p);
    return 0;
}