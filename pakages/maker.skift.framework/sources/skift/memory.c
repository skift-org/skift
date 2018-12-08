#include <skift/memory.h>

void sk_memory_copy(void *src, void *dest, uint size)
{
    byte *d = dest;
    byte *s = src;

    for (size_t i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
}

void sk_memory_move(void *src, void *dest, uint size)
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

void sk_memory_set(void *addr, uint size, byte value)
{
    byte *a = addr;

    for (size_t i = 0; i < size; i++)
    {
        a[i] = value;
    }
}

void sk_memory_zero(void *addr, uint size)
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