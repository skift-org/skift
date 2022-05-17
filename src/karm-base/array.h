#pragma once

#include "_prelude.h"

#include "clamp.h"
#include "iter.h"
#include "slice.h"
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

    constexpr operator MutSlice<T>() {
        return MutSlice<T>(_buf, N);
    }
    constexpr operator Slice<T>() const {
        return Slice<T>(_buf, N);
    }

    constexpr MutSlice<T> sub(size_t start, size_t end) {
        return MutSlice<T>(_buf + start, clamp(end, start, N));
    }

    constexpr Slice<T> sub(size_t start, size_t end) const {
        return Slice<T>(_buf + start, clamp(end, start, N));
    }

    template <size_t L>
    constexpr Ordr cmp(Array<T, L> const &other) const {
        return Op::cmp(_buf, N, other._buf, L);
    }

    constexpr Ordr cmp(Slice<T> const &other) const {
        return Op::cmp(_buf, N, other._buf, other._len);
    }

    constexpr T *buf() { return _buf; }

    constexpr T const *buf() const { return _buf; }

    constexpr size_t len() const { return N; }

    constexpr auto iter() const {
        return Iter{[&, i = 0uz]() mutable -> Opt<T> {
            if (i >= N) {
                return NONE;
            }

            return _buf[i++];
        }};
    }

    constexpr auto iter_rev() const {
        return Iter{[&, i = N]() mutable -> Opt<T> {
            if (i == 0) {
                return NONE;
            }

            return _buf[--i];
        }};
    }
};

} // namespace Karm::Base
