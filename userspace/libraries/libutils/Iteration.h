#pragma once

#include <libutils/Callback.h>

namespace Utils
{

enum class Iteration
{
    CONTINUE,
    STOP,
};

template <typename T>
using IterationCallback = Callback<Iteration(T)>;

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
