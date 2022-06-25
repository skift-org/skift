#pragma once

#include "iter.h"
#include "ordr.h"

namespace Karm {

using Byte = uint8_t;

template <typename T>
struct SimpleSlice {
    T const *_buf = nullptr;
    size_t _len = 0;

    constexpr T const *buf() const { return _buf; }
    constexpr size_t len() const { return _len; }
};

template <typename T>
struct SimpleMutSlice {
    T *_buf = nullptr;
    size_t _len = 0;

    constexpr T const *buf() const { return _buf; }
    constexpr T *buf() { return _buf; }
    constexpr size_t len() const { return _len; }
};

template <typename T>
struct Sliceable {
    virtual ~Sliceable() = default;

    virtual constexpr T const *buf() const = 0;

    virtual constexpr size_t len() const = 0;

    operator T const *() const {
        return buf();
    }

    bool empty() const { return len() == 0; }

    T const &peekFront() const { return at(0); }

    T const &peekBack() const { return at(len() - 1); }

    template <typename Out = SimpleSlice<T>>
    constexpr Out sub(size_t start, size_t size) const {
        if (start >= len()) {
            return {};
        }

        return {buf() + start, clamp(size, 0uz, len() - start)};
    }

    template <typename Out = SimpleSlice<T>>
    constexpr Out sub(size_t start) const {
        if (start >= len()) {
            return {};
        }

        return sub(start, len() - start);
    }

    constexpr T const &at(size_t index) const { return buf()[index]; }

    constexpr T const &operator[](size_t i) const {
        return buf()[i];
    }

    constexpr Ordr cmp(Sliceable<T> const &other) const {
        return Op::cmp(buf(), len(), other.buf(), other.len());
    }

    constexpr size_t size() const { return len() * sizeof(T); }

    T const *begin() const { return buf(); }

    T const *end() const { return buf() + len(); }

    template <typename Self>
    static constexpr auto _iter(Self *self) {
        return Iter([self, i = 0uz]() mutable -> T * {
            if (i >= self->_len) {
                return NONE;
            }

            return &self->_buf[i++].unwrap();
        });
    }

    template <typename Self>
    static constexpr auto _iterRev(Self *self) {
        return Iter([self, i = self->_len - 1]() mutable -> T const * {
            if (i < 0) {
                return NONE;
            }

            return &self->_buf[i--].unwrap();
        });
    }

    constexpr auto iter() const { return _iter(this); }

    constexpr auto iterRev() const { return _iterRev(this); }

    template <typename Dest = SimpleMutSlice<T>>
    constexpr size_t copyTo(Dest dst) const {
        size_t copied = 0;
        for (auto &elem : *this) {
            dst.buf()[copied++] = elem;
        }
        return copied;
    }

    template <typename Out = SimpleSlice<Byte>>
    constexpr Out bytes() const {
        return {reinterpret_cast<Byte const *>(buf()), size()};
    }

    template <typename U, typename Out = SimpleSlice<U>>
    constexpr Out cast() const {
        return {reinterpret_cast<U const *>(buf()), size() / sizeof(U)};
    }
};

template <typename T>
struct MutSliceable : public Sliceable<T> {
    using Sliceable<T>::buf;
    using Sliceable<T>::len;
    using Sliceable<T>::size;
    using Sliceable<T>::peekFront;
    using Sliceable<T>::peekBack;

    virtual ~MutSliceable() = default;

    virtual constexpr T *buf() = 0;

    operator T *() {
        return buf();
    }

    T &peekFront() { return at(0); }

    T &peekBack() { return at(len() - 1); }

    template <typename Out = SimpleMutSlice<T>>
    constexpr Out sub(size_t start, size_t size) {
        if (start >= len()) {
            return {nullptr, 0uz};
        }

        return {buf() + start, clamp(size, 0uz, len() - start)};
    }

    template <typename Out = SimpleMutSlice<T>>
    constexpr Out sub(size_t start) {
        if (start >= len()) {
            return {nullptr, 0uz};
        }

        return sub(start, len() - start);
    }

    constexpr T &at(size_t index) { return buf()[index]; }

    constexpr T &operator[](size_t i) {
        return buf()[i];
    }

    constexpr T *begin() { return buf(); }

    constexpr T *end() { return buf() + len(); }

    constexpr auto iter() { return _iter(this); }

    constexpr auto iterRev() { return _iterRev(this); }

    constexpr size_t copyFrom(Sliceable<T> &from) {
        size_t l = min(from.len, len());
        for (size_t i = 0; i < l; i++) {
            at(i) = from.buf[i];
        }
        return l;
    }

    constexpr size_t moveFrom(MutSliceable<T> &from) {
        size_t l = min(from.len, len());
        for (size_t i = 0; i < l; i++) {
            at(i) = std::move(from.buf[i]);
        }
        return l;
    }

    constexpr size_t fill(T value) {
        for (size_t i = 0; i < len(); i++) {
            at(i) = value;
        }
        return len();
    }

    constexpr size_t zero() {
        return fill(T{});
    }

    constexpr void reverse() {
        for (size_t i = 0; i < len() / 2; i++) {
            std::swap(at(i), at(len() - i - 1));
        }
    }

    template <typename Out = SimpleMutSlice<Byte>>
    constexpr Out mutBytes() {
        return {reinterpret_cast<Byte *>(buf()), size()};
    }

    template <typename U, typename Out = SimpleSlice<U>>
    constexpr Out cast() {
        return {reinterpret_cast<U *>(buf()), size() / sizeof(U)};
    }
};

template <typename T>
struct Slice : public Sliceable<T> {
    using Inner = T;

    T const *_buf{};
    size_t _len{};

    constexpr Slice() = default;

    constexpr Slice(T const *buf, size_t len)
        : _buf(buf), _len(len) {}

    constexpr Slice(T const *begin, T const *end)
        : Slice(begin, end - begin) {}

    constexpr Slice(Sliceable<T> &other)
        : Slice(other.buf(), other.len()) {}

    constexpr Slice(SimpleSlice<T> other)
        : Slice(other.buf(), other.len()) {}

    constexpr Slice(SimpleMutSlice<T> other)
        : Slice(other.buf(), other.len()) {}

    constexpr T const *buf() const override { return _buf; }

    constexpr size_t len() const override { return _len; }
};

template <typename T>
struct MutSlice : public MutSliceable<T> {
    using Inner = T;

    T *_buf{};
    size_t _len{};

    constexpr MutSlice() = default;

    constexpr MutSlice(T *buf, size_t len)
        : _buf(buf), _len(len) {}

    constexpr MutSlice(T *begin, T *end)
        : MutSlice(begin, end - begin) {}

    constexpr MutSlice(MutSliceable<T> &other)
        : MutSlice(other.buf(), other.len()) {}

    constexpr MutSlice(SimpleMutSlice<T> other)
        : MutSlice(other.buf(), other.len()) {}

    constexpr T *buf() override { return _buf; }
    constexpr T const *buf() const override { return _buf; }
    constexpr size_t len() const override { return _len; }
};

using Bytes = Slice<Byte>;

using MutBytes = MutSlice<Byte>;

} // namespace Karm
