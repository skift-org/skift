#pragma once

#include <libutils/Traits.h>
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

namespace std
{

using nullptr_t = decltype(nullptr);

template <typename T>
constexpr typename RemoveReference<T>::Type &&move(T &&arg)
{
    return static_cast<typename RemoveReference<T>::Type &&>(arg);
}

template <typename T>
constexpr T &&forward(typename RemoveReference<T>::Type &param)
{
    return static_cast<T &&>(param);
}

template <typename T>
constexpr T &&forward(typename RemoveReference<T>::Type &&param)
{
    return static_cast<T &&>(param);
}

template <typename T>
void swap(T &left, T &right)
{
    T tmp = move(left);
    left = move(right);
    right = move(tmp);
}

template <typename T, typename U = T>
constexpr T exchange(T &slot, U &&value)
{
    T old = move(slot);
    slot = forward<U>(value);
    return old;
}

template <typename T>
struct initializer_list
{
private:
    T *_data;
    size_t _size;

    constexpr initializer_list(const T *data, size_t size)
        : _data(data), _size(size)
    {
    }

public:
    size_t size() const { return _size; }

    constexpr initializer_list()
        : _data(0), _size(0)
    {
    }

    const T &operator[](size_t index) const
    {
        return _data[index];
    }

    T &operator[](size_t index)
    {
        return _data[index];
    }

    constexpr const T *begin() const { return _data; }

    constexpr const T *end() const { return _data + _size; }
};

} // namespace std