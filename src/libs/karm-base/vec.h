#pragma once

#include "buf.h"
#include "opt.h"
#include "ref.h"
#include "std.h"

namespace Karm {

template <typename S, typename T = typename S::Inner>
struct _Vec {
    using Inner = T;

    S _buf{};

    constexpr _Vec() = default;

    _Vec(usize cap) : _buf(cap) {}

    _Vec(std::initializer_list<T> other) : _buf(other) {}

    _Vec(Sliceable<T> auto &other) : _buf(other) {}

    _Vec(S storage) : _buf(storage) {}

    /* --- Collection --- */

    void add(T const &val) {
        pushBack(val);
    }

    void add(T &&val) {
        pushBack(std::move(val));
    }

    bool contains(T const &val) const {
        for (auto const &v : *this) {
            if (v == val) {
                return true;
            }
        }

        return false;
    }

    bool removeAll(T const &val) {
        bool changed = false;

        for (usize i = 1; i < _buf.len() + 1; i++) {
            if (_buf[i - 1] == val) {
                _buf.removeAt(i - 1);
                changed = true;
                i--;
            }
        }

        return changed;
    }

    /* --- Capacity --- */

    void ensure(usize cap) { _buf.ensure(cap); }

    void truncate(usize len) { _buf.truncate(len); }

    void resize(usize len, T fill = {}) { _buf.resize(len, fill); }

    void fit() { _buf.fit(); }

    void clear() { _buf.truncate(0); }

    usize cap() const { return _buf.cap(); }

    /* --- Random Access --- */

    void insert(usize index, T const &value) { _buf.insert(index, T(value)); }

    void insert(usize index, T &&value) { _buf.insert(index, std::move(value)); }

    void replace(usize index, T const &value) { _buf[index] = T(value); }

    void replace(usize index, T &&value) { _buf[index] = std::move(value); }

    T removeAt(usize index) { return _buf.removeAt(index); }

    void removeRange(usize index, usize count) { _buf.removeRange(index, count); }

    /* --- Front Access --- */

    T const &peekFront() const { return _buf[0]; }

    void pushFront(T const &value) { _buf.insert(0, T(value)); }

    void pushFront(T &&value) { _buf.insert(0, std::move(value)); }

    void pushFront(Sliceable<T> auto &other) {
        for (auto &v : other) {
            pushFront(v);
        }
    }

    template <typename... Args>
    void emplaceFront(Args &&...args) { _buf.emplace(0, std::forward<Args>(args)...); }

    T popFront() { return _buf.removeAt(0); }

    /* --- Back Access --- */

    T const &peekBack() const { return _buf[_buf.len() - 1]; }

    void pushBack(T const &value) { insert(len(), value); }

    void pushBack(T &&value) { insert(len(), std::move(value)); }

    void pushBack(Sliceable<T> auto &other) {
        for (auto &v : other) {
            pushBack(v);
        }
    }

    template <typename... Args>
    void emplaceBack(Args &&...args) { _buf.emplace(len(), std::forward<Args>(args)...); }

    T popBack() { return removeAt(len() - 1); }

    /* --- MutSliceable --- */

    constexpr usize len() const { return _buf.len(); }

    constexpr T *buf() { return _buf.buf(); }

    constexpr T const *buf() const { return _buf.buf(); }

    constexpr T &operator[](usize i) {
        if (i >= len()) {
            panic("index out of bounds");
        }
        return _buf[i];
    }

    constexpr T const &operator[](usize i) const {
        if (i >= len()) {
            panic("index out of bounds");
        }
        return _buf[i];
    }
};

template <typename T>
using Vec = _Vec<Buf<T>>;

template <typename T, usize N>
using InlineVec = _Vec<InlineBuf<T, N>>;

} // namespace Karm
