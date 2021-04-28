#pragma once

#include <libutils/Traits.h>

namespace Utils
{
template <class T>
inline void swap(T &a, T &b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

} // namespace Utils
