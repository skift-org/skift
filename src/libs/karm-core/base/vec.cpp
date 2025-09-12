module;

#include <karm-core/macros.h>

export module Karm.Core:base.vec;

import :base.opt;
import :base.buf;

namespace Karm {

export template <typename S, typename T = typename S::Inner>
struct _Vec {
    using Inner = T;

    S _buf{};

    constexpr _Vec() = default;

    _Vec(usize cap) : _buf(cap) {}

    _Vec(std::initializer_list<T> other) : _buf(other) {}

    _Vec(Sliceable<T> auto const& other) : _buf(other) {}

    _Vec(S storage) : _buf(storage) {}

    // MARK: Collection

    bool removeAll(Meta::Equatable<T> auto const& val) {
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

    // MARK: Capacity

    void ensure(usize cap) { _buf.ensure(cap); }

    void trunc(usize len) { _buf.trunc(len); }

    void resize(usize len, T fill = {}) { _buf.resize(len, fill); }

    void fit() { _buf.fit(); }

    void clear() { _buf.trunc(0); }

    usize cap() const { return _buf.cap(); }

    // MARK: Random Access

    void insert(usize index, T const& value) { _buf.insert(index, T(value)); }

    void insert(usize index, T&& value) { _buf.insert(index, std::move(value)); }

    void insertMany(usize index, Sliceable<T> auto const& other) {
        for (auto& v : other) {
            insert(index++, v);
        }
    }

    void replace(usize index, T const& value) { _buf[index] = T(value); }

    void replace(usize index, T&& value) { _buf[index] = std::move(value); }

    T removeAt(usize index) { return _buf.removeAt(index); }

    void removeRange(usize index, usize count) { _buf.removeRange(index, count); }

    void removeUnordered(usize index) {
        if (len() <= 1) [[unlikely]] {
            clear();
            return;
        }

        std::swap(_buf[index], _buf[_buf.len() - 1]);
        _buf.trunc(_buf.len() - 1);
    }

    // MARK: Front Access

    void pushFront(T const& value) { _buf.insert(0, T(value)); }

    void pushFront(T&& value) { _buf.insert(0, std::move(value)); }

    void pushFront(Sliceable<T> auto& other) {
        for (auto& v : iterRev(other))
            pushFront(v);
    }

    template <typename... Args>
    T& emplaceFront(Args&&... args) lifetimebound {
        return _buf.emplace(0, std::forward<Args>(args)...);
    }

    T popFront() { return _buf.removeAt(0); }

    // MARK: Back Access

    void pushBack(T const& value) { insert(len(), value); }

    void pushBack(T&& value) { insert(len(), std::move(value)); }

    void pushBack(Sliceable<T> auto& other) {
        for (auto& v : other)
            pushBack(v);
    }

    template <typename... Args>
    T& emplaceBack(Args&&... args) lifetimebound {
        return _buf.emplace(len(), std::forward<Args>(args)...);
    }

    T popBack() {
        return removeAt(len() - 1);
    }

    // MARK: MutSliceable

    constexpr usize len() const { return _buf.len(); }

    constexpr T* buf() lifetimebound { return _buf.buf(); }

    constexpr T const* buf() const lifetimebound { return _buf.buf(); }

    constexpr T& operator[](usize i) lifetimebound {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");

        return _buf[i];
    }

    constexpr T const& operator[](usize i) const lifetimebound {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");

        return _buf[i];
    }

    constexpr explicit operator bool() const { return len(); }
};

export template <typename T>
using Vec = _Vec<Buf<T>>;

export template <typename T, usize N>
using InlineVec = _Vec<InlineBuf<T, N>>;

export template <Nicheable T>
struct Niche<_Vec<T>> {
    struct Content : public Niche<T>::Content {};
};

} // namespace Karm
