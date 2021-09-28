#pragma once

#include <libutils/Func.h>

namespace Utils
{

enum struct Iter
{
    CONTINUE,
    STOP,
};

template <typename T>
using IterFunc = Func<Iter(T)>;

template <typename T>
struct ContiguousIterator
{
private:
    T *_ptr;

public:
    ContiguousIterator(T *ptr) : _ptr(ptr) {}

    ContiguousIterator<T> operator++()
    {
        ++_ptr;
        return *this;
    }

    bool operator!=(const ContiguousIterator<T> &other) const
    {
        return _ptr != other._ptr;
    }

    const T &operator*() const
    {
        return *_ptr;
    }
};

} // namespace Utils
