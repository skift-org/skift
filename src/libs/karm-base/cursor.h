#pragma once

#include "slice.h"

namespace Karm {

template <typename T>
struct Cursor {
    using Inner = T;

    T const *_begin = nullptr;
    T const *_end = nullptr;

    constexpr Cursor() = default;

    ALWAYS_INLINE constexpr Cursor(Sliceable<T> auto &slice)
        : _begin(begin(slice)), _end(end(slice)) {}

    ALWAYS_INLINE constexpr T const &operator[](usize i) const {
        return _begin[i];
    }

    ALWAYS_INLINE constexpr operator T const *() const { return _begin; }

    ALWAYS_INLINE constexpr bool ended() const {
        return _begin >= _end;
    }

    ALWAYS_INLINE constexpr usize rem() const {
        return _end - _begin;
    }

    ALWAYS_INLINE constexpr T curr() const {
        if (not ended()) {
            panic("curr() called on ended cursor");
        }
        return *_begin;
    }

    ALWAYS_INLINE constexpr T next() {
        if (ended()) {
            panic("next() called on ended cursor");
        }
        T r = *_begin;
        _begin++;
        return r;
    }

    ALWAYS_INLINE constexpr void next(usize n) {
        for (usize i = 0; i < n; i++)
            next();
    }

    ALWAYS_INLINE constexpr T const *buf() const {
        return _begin;
    }

    ALWAYS_INLINE constexpr usize len() const {
        return _end - _begin;
    }

    ALWAYS_INLINE constexpr Bytes bytes() const {
        return Bytes{_begin, _end};
    }

    ALWAYS_INLINE constexpr MutBytes bytes() {
        return MutBytes{_begin, _end};
    }
};

template <typename T>
struct MutCursor {
    T *_begin = nullptr;
    T *_end = nullptr;

    ALWAYS_INLINE constexpr MutCursor(MutSliceable<T> auto &slice)
        : _begin(begin(slice)), _end(end(slice)) {}

    ALWAYS_INLINE constexpr T &operator[](usize i) {
        return _begin[i];
    }

    ALWAYS_INLINE constexpr T const &operator[](usize i) const {
        return _begin[i];
    }

    ALWAYS_INLINE constexpr operator T *() { return _begin; }

    ALWAYS_INLINE constexpr operator T const *() const { return _begin; }

    ALWAYS_INLINE bool ended() const {
        return _begin == _end;
    }

    ALWAYS_INLINE constexpr usize rem() const {
        return _end - _begin;
    }

    ALWAYS_INLINE constexpr T curr() const {
        return *_begin;
    }

    ALWAYS_INLINE constexpr T next() {
        return *_begin++;
    }

    ALWAYS_INLINE constexpr bool put(T c) {
        if (_begin == _end) {
            return true;
        }

        *_begin++ = c;
        return false;
    }

    ALWAYS_INLINE constexpr void skip() {
        ++_begin;
    }

    ALWAYS_INLINE constexpr void skip(usize n) {
        _begin += n;
    }

    ALWAYS_INLINE constexpr T *buf() {
        return _begin;
    }

    ALWAYS_INLINE constexpr T const *buf() const {
        return _begin;
    }

    ALWAYS_INLINE constexpr usize len() const {
        return _end - _begin;
    }
};

} // namespace Karm
