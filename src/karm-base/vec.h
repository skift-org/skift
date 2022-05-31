#pragma once

#include "_prelude.h"

#include "buf.h"
#include "iter.h"
#include "opt.h"
#include "ref.h"

namespace Karm {

template <typename S>
struct _Vec {
    using T = typename S::T;

    S _buf;

    constexpr _Vec() = default;

    _Vec(size_t cap) : _buf(cap) {}

    _Vec(std::initializer_list<T> &&other) : _buf(std::forward<std::initializer_list<T>>(other)) {}

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

        for (size_t i = 0; i < _buf.len; i++) {
            if (_buf.peek(i) == val) {
                _buf.remove(i);
                changed = true;
            }
        }

        return changed;
    }

    /* --- Capacity --- */

    void ensure(size_t cap) { _buf.ensure(cap); }

    void truncate(size_t len) { _buf.truncate(len); }

    void fit() { _buf.fit(); }

    void clear() { _buf.truncate(0); }

    size_t cap() const { return _buf.cap(); }

    /* --- Random Access --- */

    T &at(size_t index) { return _buf.at(index); }

    T const &at(size_t index) const { return _buf.at(index); }

    void insert(size_t index, T const &value) { _buf.insert(index, T(value)); }

    T removeAt(size_t index) { return _buf.removeAt(index, 1); }

    void removeAt(size_t index, size_t count) { _buf.removeAt(index, count); }

    /* --- Front Access --- */

    T &peekFront() { return _buf.at(0); }

    T const &peekFront() const { return _buf.at(0); }

    void pushFront(T const &value) { _buf.insert(T(value)); }

    void pushFront(T &&value) { _buf.insert(std::forward<T>(value)); }

    template <typename... Args>
    void emplaceFront(Args &&...args) { _buf.emplace((args)...); }

    T popFront() { return _buf.removeAt(0); }

    /* --- Back Access --- */

    T const &peekBack() const { return _buf.at(len() - 1); }

    T &peekBack() { return _buf.at(len() - 1); }

    void pushBack(T const &value) { insert(len(), value); }

    void pushBack(T &&value) { insert(len(), std::forward<T>(value)); }

    template <typename... Args>
    void emplaceBack(Args &&...args) { insert(len(), T(std::forward<Args>(args)...)); }

    T popBack() { return removeAt(len() - 1); }

    /* --- Iteration --- */

    static auto _iter(auto self) {
        return Iter([self, i = 0uz]() mutable {
            if (i >= self->len()) {
                return nullptr;
            }
            return self->buf() + i;
        });
    }

    static auto _iterRev(auto self) {
        return Iter([self, i = self->len()]() mutable {
            if (i == 0) {
                return nullptr;
            }

            i--;
            return self->buf() + i;
        });
    }

    constexpr auto iter() { return _iter(this); }

    constexpr auto iter() const { return _iter(this); }

    constexpr auto iterRev() { return _iterRev(this); }

    constexpr auto iterRev() const { return _iterRev(this); }

    /* --- Len & Buf --- */

    size_t len() const { return _buf.len(); }

    T *buf() { return _buf.buf(); }

    T const *buf() const { return _buf.buf(); }

    T *begin() { return buf(); }

    T *end() { return buf() + len(); }

    T const *begin() const { return buf(); }

    T const *end() const { return buf() + end(); }
};

template <typename T>
using Vec = _Vec<Buf<T>>;

template <typename T, size_t N>
using InlineVec = _Vec<InlineBuf<T, N>>;

} // namespace Karm
