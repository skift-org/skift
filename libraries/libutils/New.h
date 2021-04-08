#pragma once

#include <stddef.h>

#if __CONFIG_IS_HOSTED__ == 0
inline void *operator new(size_t, void *ptr)
{
    return ptr;
}

inline void *operator new[](size_t, void *ptr)
{
    return ptr;
}
#else
#    include <new>
#endif