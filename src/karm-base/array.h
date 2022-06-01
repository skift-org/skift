#pragma once

#include "_prelude.h"

#include "clamp.h"
#include "iter.h"
#include "slice.h"
#include "std.h"

namespace Karm {

template <typename _T, size_t N>
struct Array {
    using T = _T;

    T _buf[N] = {};

    constexpr Array() = default;

    constexpr Array(std::initializer_list<T> init) {
        size_t i = 0;
        for (auto &elem : init) {
            _buf[i++] = elem;
        }
    }

    /* --- Random Access --- */

    constexpr T &at(size_t index) { return _buf[index]; }

    constexpr T const &at(size_t index) const { return _buf[index]; }

    constexpr T &operator[](size_t i) { return at(i); }

    constexpr T const &operator[](size_t i) const { return at(i); }

    /* --- Slicing --- */

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

    /* --- Comparison --- */

    constexpr Ordr cmp(Slice<T> const &other) const {
        return Op::cmp(_buf, N, other._buf, other._len);
    }

    /* --- Iteration --- */

    template <typename Self>
    constexpr static auto _iter(Self *self) {
        return Iter([self, i = 0uz]() mutable -> T * {
            if (i >= N) {
                return nullptr;
            }

            return &self->_buf[i++];
        });
    }

    template <typename Self>
    constexpr static auto _iterRev(Self *self) {
        return Iter([self, i = N]() mutable -> T * {
            if (i == 0) {
                return NONE;
            }

            i--;
            return &self->_buf[i];
        });
    }

    constexpr auto iter() { return _iter(this); }

    constexpr auto iter() const { return _iter(this); }

    constexpr auto iterRev() { return _iterRev(this); }

    constexpr auto iterRev() const { return _iterRev(this); }

    /* --- Len & Buf --- */

    constexpr size_t len() const { return N; }

    constexpr T *buf() { return _buf; }

    constexpr T const *buf() const { return _buf; }

    constexpr T *begin() { return buf(); }

    constexpr T const *begin() const { return buf(); }

    constexpr T *end() { return buf() + len(); }

    constexpr T const *end() const { return buf() + end(); }
};

} // namespace Karm
