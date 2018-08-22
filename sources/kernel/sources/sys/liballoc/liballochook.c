// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include "liballoc.h"
#include "sync/atomic.h"
#include "kernel/memory.h"
#include "kernel/logging.h"

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
    void* p = memory_alloc(size);
    debug("Liballoc hook alloc size: %d at 0x%x.", size, p);
    return p;
}

int liballoc_free(void* p,size_t size)
{
    debug("Liballoc alloc free: %d at 0x%x.", size, p);
    memory_free(p, size);
    return 0;
}