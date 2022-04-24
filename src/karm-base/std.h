#pragma once

#include <karm-meta/ref.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

inline void *operator new(size_t, void *ptr) { return ptr; }

inline void *operator new[](size_t, void *ptr) { return ptr; }

namespace std
{

template <typename T>
Karm::Meta::RemoveRef<T> &&move(T &&t) noexcept
{
    return static_cast<Karm::Meta::RemoveRef<T> &&>(t);
}

template <typename T>
constexpr T &&forward(Karm::Meta::RemoveRef<T> &param)
{
    return static_cast<T &&>(param);
}

template <typename T>
constexpr T &&forward(Karm::Meta::RemoveRef<T> &&param)
{
    return static_cast<T &&>(param);
}

template <typename T, typename U = T>
constexpr T exchange(T &slot, U &&value)
{
    T old = move(slot);
    slot = forward<U>(value);
    return old;
}

template <typename T>
void swap(T &lhs, T &rhs)
{
    T tmp = move(lhs);
    lhs = move(rhs);
    rhs = move(tmp);
}

template <typename T>
struct initializer_list
{
    T *_buf;
    size_t _len;

    constexpr initializer_list(T const *data, size_t size)
        : _buf(data), _len(size) {}

    constexpr size_t len() const { return _len; }

    constexpr initializer_list() : _buf(0), _len(0) {}

    constexpr T const &operator[](size_t index) const { return _buf[index]; }

    constexpr T &operator[](size_t index) { return _buf[index]; }

    constexpr T const *begin() const { return _buf; }

    constexpr T const *end() const { return _buf + _len; }
};

} // namespace std
