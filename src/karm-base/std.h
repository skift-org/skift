#pragma once

#include <karm-meta/ref.h>

#include "_prelude.h"

#ifndef __skift__

#    include <new>
#    include <utility>

#    include <initializer_list>

#else

inline void *operator new(size_t, void *ptr) { return ptr; }

inline void *operator new[](size_t, void *ptr) { return ptr; }

namespace std {

template <typename T>
constexpr Karm::Meta::RemoveRef<T> &&move(T &&t) noexcept {
    return static_cast<Karm::Meta::RemoveRef<T> &&>(t);
}

template <typename T>
constexpr T &&forward(Karm::Meta::RemoveRef<T> &param) {
    return static_cast<T &&>(param);
}

template <typename T>
constexpr T &&forward(Karm::Meta::RemoveRef<T> &&param) {
    return static_cast<T &&>(param);
}

template <typename T, typename U = T>
constexpr T exchange(T &slot, U &&value) {
    T old = move(slot);
    slot = forward<U>(value);
    return old;
}

template <typename T>
constexpr void swap(T &lhs, T &rhs) {
    T tmp = move(lhs);
    lhs = move(rhs);
    rhs = move(tmp);
}

template <typename T>
struct initializer_list {
    T *_data;
    size_t _size;

    constexpr initializer_list(T const *data, size_t size) : _data(data), _size(size) {}

    constexpr initializer_list() : _data(0), _size(0) {}

    constexpr T const &operator[](size_t index) const { return _data[index]; }

    constexpr T const *begin() const { return _data; }

    constexpr T const *end() const { return _data + _size; }

    constexpr size_t size() const { return _size; }

    constexpr bool empty() const { return _size == 0; }
};

} // namespace std

#endif
