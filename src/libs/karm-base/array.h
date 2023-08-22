#pragma once

#include <karm-meta/traits.h>

#include "clamp.h"
#include "iter.h"
#include "slice.h"
#include "std.h"

namespace Karm {

template <typename T, usize N>
struct Array {
    using Inner = T;

    T _buf[N];

    constexpr T &operator[](usize i) {
        if (i >= N) {
            panic("index out of range");
        }
        return _buf[i];
    }

    constexpr T const &operator[](usize i) const {
        if (i >= N) {
            panic("index out of range");
        }
        return _buf[i];
    }

    constexpr usize len() const { return N; }

    constexpr T *buf() { return _buf; }

    constexpr T const *buf() const { return _buf; }

    constexpr bool operator==(Array const &other) const {
        for (usize i = 0; i < N; i++) {
            if (_buf[i] != other._buf[i]) {
                return false;
            }
        }
        return true;
    }

    constexpr Array reversed() const {
        Array res{};
        for (usize i = 0; i < N; i++) {
            res[i] = _buf[N - i - 1];
        }
        return res;
    }

    constexpr Bytes bytes() const {
        return {reinterpret_cast<Byte const *>(buf()), len() * sizeof(T)};
    }

    constexpr MutBytes mutBytes() {
        return {reinterpret_cast<Byte *>(buf()), len() * sizeof(T)};
    }
};

template <class T, class... U>
Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

static_assert(MutSliceable<Array<u8, 16>>);
static_assert(Meta::Pod<Array<u8, 16>>);

template <typename T, usize... Is>
constexpr Array<T, sizeof...(Is)> _makeArray(T value, std::index_sequence<Is...>) {
    return {{(static_cast<void>(Is), value)...}};
}

template <typename T, usize N>
constexpr Array<T, N> makeArray(T value) {
    return _makeArray(value, std::make_index_sequence<N>());
}

} // namespace Karm
