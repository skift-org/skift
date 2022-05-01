#pragma once

#include <karm-meta/ref.h>

#include "_prelude.h"

inline auto operator new(size_t, void *ptr) -> void * { return ptr; }

inline auto operator new[](size_t, void *ptr) -> void * { return ptr; }

namespace std {

template <typename T>
constexpr auto move(T &&t) noexcept -> Karm::Meta::RemoveRef<T> && {
    return static_cast<Karm::Meta::RemoveRef<T> &&>(t);
}

template <typename T>
constexpr auto forward(Karm::Meta::RemoveRef<T> &param) -> T && {
    return static_cast<T &&>(param);
}

template <typename T>
constexpr auto forward(Karm::Meta::RemoveRef<T> &&param) -> T && {
    return static_cast<T &&>(param);
}

template <typename T, typename U = T>
constexpr auto exchange(T &slot, U &&value) -> T {
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

    constexpr auto operator[](size_t index) const -> T const & { return _data[index]; }

    constexpr auto begin() const -> T const * { return _data; }

    constexpr auto end() const -> T const * { return _data + _size; }

    constexpr auto size() const -> size_t { return _size; }

    constexpr auto empty() const -> bool { return _size == 0; }
};

} // namespace std
