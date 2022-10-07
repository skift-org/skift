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
};

template <class T, class... U>
Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

} // namespace Karm
