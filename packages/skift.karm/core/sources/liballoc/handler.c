#include <skift/runtime.h>
#include <skift/__plugs__.h>

int liballoc_lock()
{
    return __plug_memalloc_lock();
}

int liballoc_unlock()
{
    return __plug_memalloc_unlock();
}

void* liballoc_alloc(size_t size)
{
    return __plug_memalloc_alloc(size);
}

int liballoc_free(void* p,size_t size)
{
    return __plug_memalloc_free(p, size);
}