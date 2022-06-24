#pragma once

#include "_prelude.h"

#include "clamp.h"
#include "iter.h"
#include "slice.h"
#include "std.h"

namespace Karm {

template <typename _T, size_t N>
struct Array : public MutSliceable<_T> {
    using T = _T;

    T _buf[N] = {};

    constexpr Array() = default;

    constexpr Array(std::initializer_list<T> init) {
        size_t i = 0;
        for (auto &elem : init) {
            _buf[i++] = elem;
        }
    }

    constexpr Array(Sliceable<T> &other) {
        size_t i = 0;
        for (auto &elem : other) {
            _buf[i++] = elem;
        }
    }

    /* --- MutSliceable --- */

    constexpr size_t len() const override { return N; }

    constexpr T *buf() override { return _buf; }

    constexpr T const *buf() const override { return _buf; }
};

} // namespace Karm
