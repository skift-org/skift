#pragma once

#include "defer.h"
#include "slice.h"

namespace Karm {

#pragma clang unsafe_buffer_usage begin

template <typename T>
struct Cursor {
    using Inner = T;

    T const *_begin = nullptr;
    T const *_end = nullptr;

    constexpr Cursor() = default;

    always_inline constexpr Cursor(None)
        : Cursor() {}

    always_inline constexpr Cursor(T const *ptr)
        : Cursor(ptr, ptr ? 1 : 0) {
    }

    always_inline constexpr Cursor(T const *ptr, usize len)
        : _begin(ptr), _end(ptr + len) {
        if (_begin == nullptr and _begin != _end) [[unlikely]]
            panic("null pointer with non-zero length");
    }

    always_inline constexpr Cursor(T const *begin, T const *end)
        : _begin(begin), _end(end) {
    }

    always_inline constexpr Cursor(Sliceable<T> auto &slice)
        : Cursor{begin(slice), end(slice)} {}

    always_inline constexpr Cursor(Slice<T> slice)
        : Cursor{begin(slice), end(slice)} {}

    always_inline constexpr Cursor(MutSlice<T> slice)
        : Cursor{begin(slice), end(slice)} {}

    always_inline constexpr T const &operator[](usize i) const {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    always_inline constexpr operator T const *() const {
        return _begin;
    }

    always_inline constexpr bool ended() const {
        return _begin >= _end;
    }

    always_inline constexpr usize rem() const {
        return _end - _begin;
    }

    always_inline constexpr T const &peek(usize i = 0) const {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    always_inline constexpr T const &operator*() const {
        return peek();
    }

    always_inline constexpr T const *operator->() const {
        return &peek();
    }

    always_inline constexpr T const &next() {
        if (ended()) [[unlikely]]
            panic("next() called on ended cursor");

        T const &r = *_begin;
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

    template <Meta::Equatable<T> U>
    always_inline constexpr bool skip(U const &c) {
        if (ended()) [[unlikely]]
            return false;

        if (peek() == c) {
            _begin++;
            return true;
        }

        return false;
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

    /// Creates a rollback point for the cursor. If not manually disarmed,
    /// the cursor's state will be restored to its position at the time of
    /// this rollback point's creation when it goes out of scope.
    auto rollbackPoint() {
        return ArmedDefer{[&, saved = *this] {
            *this = saved;
        }};
    }
};

template <typename T>
struct MutCursor {
    T *_begin = nullptr;
    T *_end = nullptr;

    constexpr MutCursor() = default;

    always_inline constexpr MutCursor(None)
        : MutCursor() {}

    always_inline constexpr MutCursor(T *ptr)
        : MutCursor(ptr, ptr ? 1 : 0) {}

    always_inline constexpr MutCursor(T *ptr, usize len)
        : _begin(ptr), _end(ptr + len) {
        if (_begin == nullptr and _begin != _end) [[unlikely]]
            panic("null pointer with non-zero length");
    }

    always_inline constexpr MutCursor(T *begin, T *end)
        : _begin(begin), _end(end) {}

    always_inline constexpr MutCursor(MutSliceable<T> auto &slice)
        : MutCursor{begin(slice), end(slice)} {}

    always_inline constexpr MutCursor(MutSlice<T> slice)
        : MutCursor{begin(slice), end(slice)} {}

    always_inline constexpr T &operator[](usize i) {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    always_inline constexpr T const &operator[](usize i) const {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    always_inline constexpr operator T *() {
        return _begin;
    }

    always_inline constexpr operator T const *() const {
        return _begin;
    }

    always_inline bool ended() const {
        return _begin == _end;
    }

    always_inline constexpr usize rem() const {
        return _end - _begin;
    }

    always_inline constexpr T &peek(usize i = 0) {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    always_inline constexpr T const &peek(usize i = 0) const {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    always_inline constexpr T &operator*() {
        return peek();
    }

    always_inline constexpr T *operator->() {
        return &peek();
    }

    always_inline constexpr T const &operator*() const {
        return peek();
    }

    always_inline constexpr T const *operator->() const {
        return &peek();
    }

    always_inline constexpr T &next() {
        if (ended()) [[unlikely]]
            panic("next() called on ended cursor");

        T &r = *_begin;
        _begin++;
        return r;
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

    always_inline constexpr Bytes bytes() const {
        return Bytes{_begin, _end};
    }

    always_inline constexpr MutBytes mutBytes() {
        return MutBytes{_begin, _end};
    }

    /// Creates a rollback point for the cursor. If not manually disarmed,
    /// the cursor's state will be restored to its position at the time of
    /// this rollback point's creation when it goes out of scope.
    auto rollbackPoint() {
        return ArmedDefer{[&, saved = *this] {
            *this = saved;
        }};
    }
};

#pragma clang unsafe_buffer_usage end

} // namespace Karm
