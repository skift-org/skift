#include <skift/memory.h>

void memory_copy(void *src, void *dest, uint size)
{
    byte *d = dest;
    byte *s = src;

    for (size_t i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
}

void memory_move(void *src, void *dest, uint size)
{
    byte *d = dest;
    byte *s = src;

    if (d < s)
    {
        for (uint i = 0; i < size; i++)
            d[i] = s[i];
    }
    else if (d > s)
    {
        for (uint i = size; i > 0; i--)
            d[i - 1] = s[i - 1];
    }
}

void memory_set(void *addr, uint size, byte value)
{
    byte *a = addr;

    for (size_t i = 0; i < size; i++)
    {
        a[i] = value;
    }
}

void memory_zero(void *addr, uint size)
{
    byte *a = addr;

    for (size_t i = 0; i < size; i++)
    {
        a[i] = 0;
    }
}

// TODO: memory allocator
// void *memory_alloc(uint size);
// void memory_free(void *addr);