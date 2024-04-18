#pragma once

#include "slice.h"

namespace Karm {

template <typename T>
struct Cursor {
    using Inner = T;

    T const *_begin = nullptr;
    T const *_end = nullptr;

    constexpr Cursor() = default;

    always_inline constexpr Cursor(Sliceable<T> auto const &slice)
        : _begin(begin(slice)), _end(end(slice)) {}

    always_inline constexpr Cursor(T const *begin, T const *end)
        : _begin(begin), _end(end) {}

    always_inline constexpr T const &operator[](usize i) const {
        return _begin[i];
    }

    always_inline constexpr operator T const *() const { return _begin; }

    always_inline constexpr bool ended() const {
        return _begin >= _end;
    }

    always_inline constexpr usize rem() const {
        return _end - _begin;
    }

    always_inline constexpr T curr() const {
        if (not ended()) [[unlikely]]
            panic("curr() called on ended cursor");

        return *_begin;
    }

    always_inline constexpr T next() {
        if (ended()) [[unlikely]]
            panic("next() called on ended cursor");

        T r = *_begin;
        _begin++;
        return r;
    }

    always_inline constexpr Slice<T> next(usize n) {
        if (n > rem()) [[unlikely]]
            panic("next() called on ended cursor");

        auto slice = Slice<T>{_begin, n};
        _begin += n;
        return slice;
    }

    always_inline constexpr T const *buf() const {
        return _begin;
    }

    always_inline constexpr usize len() const {
        return _end - _begin;
    }

    always_inline constexpr Bytes bytes() const {
        return Bytes{_begin, _end};
    }

    always_inline constexpr MutBytes bytes() {
        return MutBytes{_begin, _end};
    }
};

template <typename T>
struct MutCursor {
    T *_begin = nullptr;
    T *_end = nullptr;

    always_inline constexpr MutCursor(MutSliceable<T> auto &slice)
        : _begin(begin(slice)), _end(end(slice)) {}

    always_inline constexpr MutCursor(T *begin, T *end)
        : _begin(begin), _end(end) {}

    always_inline constexpr T &operator[](usize i) {
        return _begin[i];
    }

    always_inline constexpr T const &operator[](usize i) const {
        return _begin[i];
    }

    always_inline constexpr operator T *() { return _begin; }

    always_inline constexpr operator T const *() const { return _begin; }

    always_inline bool ended() const {
        return _begin == _end;
    }

    always_inline constexpr usize rem() const {
        return _end - _begin;
    }

    always_inline constexpr T curr() const {
        return *_begin;
    }

    always_inline constexpr T next() {
        if (ended()) [[unlikely]]
            panic("next() called on ended cursor");

        return *_begin++;
    }

    always_inline constexpr MutSlice<T> next(usize n) {
        if (n > rem()) [[unlikely]]
            panic("next() called on ended cursor");

        auto slice = MutSlice<T>{_begin, n};
        _begin += n;
        return slice;
    }

    always_inline constexpr bool put(T c) {
        if (_begin == _end) {
            return true;
        }

        *_begin++ = c;
        return false;
    }

    always_inline constexpr T *buf() {
        return _begin;
    }

    always_inline constexpr T const *buf() const {
        return _begin;
    }

    always_inline constexpr usize len() const {
        return _end - _begin;
    }
};

} // namespace Karm
