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
    T *_buf;
    size_t _len;

    constexpr initializer_list(T const *data, size_t size) : _buf(data), _len(size) {}

    constexpr initializer_list() : _buf(0), _len(0) {}

    constexpr auto operator[](size_t index) const -> T const & { return _buf[index]; }

    constexpr auto len() const -> size_t { return _len; }

    constexpr auto begin() const -> T const * { return _buf; }

    constexpr auto end() const -> T const * { return _buf + _len; }
};

} // namespace std
