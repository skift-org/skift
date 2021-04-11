#include <assert.h>
#include <libc/cxx/cxx.h>

void *__attribute__((weak)) operator new(size_t size)
{
    return malloc(size);
}

void *__attribute__((weak)) operator new[](size_t size)
{
    return malloc(size);
}

void __attribute__((weak)) operator delete(void *ptr)
{
    free(ptr);
}

void __attribute__((weak)) operator delete[](void *ptr)
{
    free(ptr);
}

void __attribute__((weak)) operator delete(void *ptr, size_t size)
{
    UNUSED(size);

    free(ptr);
}

void __attribute__((weak)) operator delete[](void *ptr, size_t size)
{
    UNUSED(size);

    free(ptr);
}

extern "C" void __attribute__((weak)) __cxa_pure_virtual()
{
    // logger_fatal("Attempt at calling a pure virtual function!");
    ASSERT_NOT_REACHED();
}
