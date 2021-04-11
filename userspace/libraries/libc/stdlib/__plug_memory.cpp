#include <assert.h>

#include <abi/Syscalls.h>

#include <skift/Lock.h>
#include <skift/Plugs.h>

static Lock _memory_lock{"memory_lock"};

void __plug_memory_lock()
{
    _memory_lock.acquire();
}

void __plug_memory_unlock()
{
    _memory_lock.release();
}

void *__plug_memory_alloc(size_t size)
{
    uintptr_t address = 0;
    assert(hj_memory_alloc(size, &address) == Result::SUCCESS);
    return (void *)address;
}

void __plug_memory_free(void *address, size_t size)
{
    UNUSED(size);
    hj_memory_free((uintptr_t)address);
}