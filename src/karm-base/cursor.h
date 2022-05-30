#pragma once

#include "slice.h"

namespace Karm {

template <typename T>
struct Cursor {
    T const *_begin = nullptr;
    T const *_end = nullptr;

    constexpr Cursor(){};

    Cursor(T const *begin, T const *end) : _begin(begin), _end(end) {}

    Cursor(T const *buf, size_t len) : _begin(buf), _end(buf + len) {}

    Cursor(Slice<T> slice) : _begin(slice.begin()), _end(slice.end()) {}

    Cursor(MutSlice<T> slice) : _begin(slice.begin()), _end(slice.end()) {}

    bool ended() const {
        return _begin >= _end;
    }

    size_t rem() const {
        return _end - _begin;
    }

    T peek() const {
        if (!ended()) {
            panic("peek() called on ended cursor");
        }
        return *_begin;
    }

    T next() {
        if (ended()) {
            panic("next() called on ended cursor");
        }
        T r = *_begin;
        _begin++;
        return r;
    }

    void next(size_t n) {
        for (size_t i = 0; i < n; i++)
            next();
    }

    operator T const *() {
        return _begin;
    }
};

template <typename T>
struct MutCursor {
    T *_begin = nullptr;
    T *_end = nullptr;

    MutCursor(T *begin, T *end) : _begin(begin), _end(end) {}

    MutCursor(T *buf, size_t len) : _begin(buf), _end(buf + len) {}

    MutCursor(MutSlice<T> slice) : _begin(slice.begin()), _end(slice.end()) {}

    bool ended() const {
        return _begin == _end;
    }

    size_t rem() const {
        return _end - _begin;
    }

    T peek() const {
        return *_begin;
    }

    T next() {
        return *_begin++;
    }

    bool put(T c) {
        if (_begin == _end) {
            return true;
        }

        *_begin++ = c;
        return false;
    }

    void skip() {
        ++_begin;
    }

    void skip(size_t n) {
        _begin += n;
    }

    operator T *() {
        return _begin;
    }

    operator T const *() const {
        return _begin;
    }
};

} // namespace Karm
