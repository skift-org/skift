#pragma once

#include "_prelude.h"

#include "clamp.h"
#include "iter.h"
#include "slice.h"
#include "std.h"

namespace Karm {

template <typename T, size_t N>
struct Array {
    using Inner = T;

    T _buf[N] = {};

    constexpr T &operator[](size_t i) {
        if (i >= N) {
            panic("index out of range");
        }
        return _buf[i];
    }

    constexpr T const &operator[](size_t i) const {
        if (i >= N) {
            panic("index out of range");
        }
        return _buf[i];
    }

    constexpr size_t len() const { return N; }

    constexpr T *buf() { return _buf; }

    constexpr T const *buf() const { return _buf; }

    constexpr bool operator==(Array const &other) const {
        for (size_t i = 0; i < N; i++) {
            if (_buf[i] != other._buf[i]) {
                return false;
            }
        }
        return true;
    }

    Bytes bytes() const {
        return {reinterpret_cast<Byte const *>(buf()), len() * sizeof(T)};
    }

    MutBytes mutBytes() {
        return {reinterpret_cast<Byte *>(buf()), len() * sizeof(T)};
    }
};

template <class T, class... U>
Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

} // namespace Karm
