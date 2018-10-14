#include <stddef.h>

#include <skift/io.h>
#include <skift/lock.h>
#include <skift/process.h>

lock_t memlock;

void stdlib_init(void)
{
    sk_lock_init(memlock);
}

int liballoc_lock()
{
    sk_lock_acquire(memlock);
    return 0;
}

int liballoc_unlock()
{
    sk_lock_release(memlock);
    return 0;
}

void* liballoc_alloc(size_t size)
{
    return (void*)sk_process_alloc(size);;
}

int liballoc_free(void* p,size_t size)
{
    return sk_process_free((unsigned int)p, size);;
}