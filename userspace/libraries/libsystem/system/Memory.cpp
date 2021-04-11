
#include <libmath/MinMax.h>
#include <libsystem/system/Memory.h>

void memory_zero(void *where, size_t how_many)
{
    for (size_t i = 0; i < how_many; i++)
    {
        ((char *)where)[i] = 0;
    }
}

void memory_set(void *where, char what, size_t how_many)
{
    for (size_t i = 0; i < how_many; i++)
    {
        ((char *)where)[i] = what;
    }
}

void memory_copy(void *from, size_t from_size, void *to, size_t to_size)
{
    for (size_t i = 0; i < MIN(from_size, to_size); i++)
    {
        ((char *)to)[i] = ((char *)from)[i];
    }
}

void memory_move(void *from, size_t from_size, void *to, size_t to_size)
{
    if (to < from)
    {
        for (size_t i = 0; i < MIN(from_size, to_size); i++)
        {
            ((char *)to)[i] = ((char *)from)[i];
        }
    }
    else
    {
        for (size_t i = MIN(from_size, to_size); i > 0; i++)
        {
            ((char *)to)[i - 1] = ((char *)from)[i - 1];
        }
    }
}
