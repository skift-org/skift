#pragma once

#include "_prelude.h"

#include "buf.h"
#include "opt.h"
#include "ref.h"

namespace Karm {

template <typename S, typename T = typename S::T>
struct _Vec {
    using Inner = T;

    S _buf{};

    constexpr _Vec() = default;

    _Vec(size_t cap) : _buf(cap) {}

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

        for (size_t i = 1; i < _buf.len() + 1; i++) {
            if (Op::eq(_buf[i - 1], val)) {
                _buf.removeAt(i - 1);
                changed = true;
                i--;
            }
        }

        return changed;
    }

    /* --- Capacity --- */

    void ensure(size_t cap) { _buf.ensure(cap); }

    void truncate(size_t len) { _buf.truncate(len); }

    void resize(size_t len, T fill = {}) { _buf.resize(len, fill); }

    void fit() { _buf.fit(); }

    void clear() { _buf.resize(0); }

    size_t cap() const { return _buf.cap(); }

    /* --- Random Access --- */

    void insert(size_t index, T const &value) { _buf.insert(index, T(value)); }

    void insert(size_t index, T &&value) { _buf.insert(index, std::move(value)); }

    void replace(size_t index, T const &value) { _buf[index] = T(value); }

    void replace(size_t index, T &&value) { _buf[index] = std::move(value); }

    T removeAt(size_t index) { return _buf.removeAt(index); }

    void removeRange(size_t index, size_t count) { _buf.removeRange(index, count); }

    /* --- Front Access --- */

    void pushFront(T const &value) { _buf.insert(T(value)); }

    void pushFront(T &&value) { _buf.insert(0, std::move(value)); }

    template <typename... Args>
    void emplaceFront(Args &&...args) { _buf.emplace((args)...); }

    T popFront() { return _buf.removeAt(0); }

    /* --- Back Access --- */

    void pushBack(T const &value) { insert(len(), value); }

    void pushBack(T &&value) { insert(len(), std::move(value)); }

    template <typename... Args>
    void emplaceBack(Args &&...args) { insert(len(), T(std::move(args)...)); }

    T popBack() { return removeAt(len() - 1); }

    /* --- MutSliceable --- */

    constexpr size_t len() const { return _buf.len(); }

    constexpr T *buf() { return _buf.buf(); }

    constexpr T const *buf() const { return _buf.buf(); }

    constexpr T &operator[](size_t i) {
        return _buf[i];
    }

    constexpr T const &operator[](size_t i) const {
        return _buf[i];
    }
};

template <typename T>
using Vec = _Vec<Buf<T>>;

template <typename T, size_t N>
using InlineVec = _Vec<InlineBuf<T, N>>;

} // namespace Karm
