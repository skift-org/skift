#pragma once

#include "_prelude.h"

#include "std.h"

namespace Karm::Base {

template <typename T, size_t N>
struct Array {
    T _buf[N];

    constexpr Array() = default;

    constexpr Array(std::initializer_list<T> init) {
        size_t i = 0;
        for (auto &elem : init) {
            _buf[i++] = elem;
        }
    }

    T &operator[](size_t i) {
        return _buf[i];
    }

    constexpr T const &operator[](size_t i) const {
        return _buf[i];
    }

    constexpr T *buf() { return _buf; }

    constexpr size_t len() { return N; }

    constexpr T *begin() { return _buf; }

    constexpr T *end() { return _buf + N; }
};

} // namespace Karm::Base
