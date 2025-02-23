#pragma once

#include <karm-meta/traits.h>

#include "slice.h"

namespace Karm {

template <typename T, usize N>
struct Array {
    using Inner = T;

    T _buf[N];

    always_inline static constexpr Array from(Sliceable<T> auto slice) {
        Array res{};
        for (usize i = 0; i < N; i++)
            res[i] = slice[i];
        return res;
    }

    always_inline static constexpr Array fill(auto f) {
        return [&]<usize... Is>(std::index_sequence<Is...>) {
            return Array{f(Is)...};
        }(std::make_index_sequence<N>());
    }

    always_inline static constexpr Array fill(T value) {
        return fill([&](usize) {
            return value;
        });
    }

    always_inline constexpr T& operator[](usize i) {
        if (i >= N) [[unlikely]]
            panic("index out of range");

        return _buf[i];
    }

    always_inline constexpr T const& operator[](usize i) const {
        if (i >= N) [[unlikely]]
            panic("index out of range");

        return _buf[i];
    }

    always_inline constexpr usize len() const { return N; }

    always_inline constexpr T* buf() { return _buf; }

    always_inline constexpr T const* buf() const { return _buf; }

    always_inline constexpr Array reversed() const {
        Array res{};
        for (usize i = 0; i < N; i++)
            res[i] = _buf[N - i - 1];
        return res;
    }

    always_inline constexpr Bytes bytes() const {
        return {reinterpret_cast<Byte const*>(buf()), len() * sizeof(T)};
    }

    always_inline constexpr MutBytes mutBytes() {
        return {reinterpret_cast<Byte*>(buf()), len() * sizeof(T)};
    }
};

template <class T, class... U>
Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

static_assert(MutSliceable<Array<u8, 16>>);
static_assert(Meta::Pod<Array<u8, 16>>);

template <typename T, usize... Is>
always_inline constexpr Array<T, sizeof...(Is)> _makeArray(T value, std::index_sequence<Is...>) {
    return {{(static_cast<void>(Is), value)...}};
}

template <typename T, usize N>
always_inline constexpr Array<T, N> makeArray(T value) {
    return _makeArray(value, std::make_index_sequence<N>());
}

} // namespace Karm
