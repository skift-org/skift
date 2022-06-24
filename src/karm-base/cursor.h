#pragma once

#include "slice.h"

namespace Karm {

template <typename T>
struct Cursor :
    public Sliceable<T> {

    T const *_begin = nullptr;
    T const *_end = nullptr;

    constexpr Cursor() = default;

    Cursor(Sliceable<T> const &slice) : _begin(slice.begin()), _end(slice.end()) {}

    bool ended() const {
        return _begin >= _end;
    }

    size_t rem() const {
        return _end - _begin;
    }

    T curr() const {
        if (!ended()) {
            panic("curr() called on ended cursor");
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

    constexpr T const *buf() const override {
        return _begin;
    }

    constexpr size_t len() const override {
        return _end - _begin;
    }
};

template <typename T>
struct MutCursor : public MutSliceable<T> {
    T *_begin = nullptr;
    T *_end = nullptr;

    MutCursor(MutSliceable<T> &slice)
        : _begin(slice.begin()), _end(slice.end()) {}

    bool ended() const {
        return _begin == _end;
    }

    size_t rem() const {
        return _end - _begin;
    }

    T curr() const {
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

    constexpr T *buf() override {
        return _begin;
    }

    constexpr T const *buf() const override {
        return _begin;
    }

    constexpr size_t len() const override {
        return _end - _begin;
    }
};

} // namespace Karm
