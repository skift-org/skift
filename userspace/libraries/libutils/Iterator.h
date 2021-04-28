#pragma once

#include <libutils/Traits.h>

namespace Utils
{
template <class Iterator>
inline void advance(Iterator &it, int64_t n = 1)
{
    while (n > 0)
    {
        --n;
        ++it;
    }
    while (n < 0)
    {
        ++n;
        --it;
    }
}

template <class Iterator>
inline Iterator prev(Iterator it, size_t n = 1)
{
    advance(it, -n);
    return it;
}

template <class Iterator>
inline Iterator next(Iterator it, size_t n = 1)
{
    advance(it, n);
    return it;
}

template <class Iterator>
inline size_t distance(Iterator first, Iterator last)
{
    size_t result = 0;
    while (first != last)
    {
        ++first;
        ++result;
    }
    return result;
}

} // namespace Utils
