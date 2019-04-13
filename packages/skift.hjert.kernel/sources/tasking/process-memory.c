#include "kernel/process.h"
#include "kernel/memory.h"

int process_memory_map(process_t* p, uint addr, uint count)
{
    return memory_map(p->pdir, addr, count, 1);
}

int process_memory_unmap(process_t* p, uint addr, uint count)
{
    return memory_unmap(p->pdir, addr, count);
}

uint process_memory_alloc(process_t* p, uint count)
{
    uint addr = memory_alloc(p->pdir, count, 1);
    sk_log(LOG_DEBUG, "Giving userspace %d memory block at 0x%08x", count, addr);
    return addr;
}

void process_memory_free(process_t* p, uint addr, uint count)
{
    sk_log(LOG_DEBUG, "Userspace free %d memory block at 0x%08x", count, addr);
    return memory_free(p->pdir, addr, count, 1);
}