#pragma once

#include "iter.h"
#include "ordr.h"

namespace Karm {

template <typename T>
struct Slice {
    T const *_buf;
    size_t _len;

    constexpr Slice()
        : _buf(nullptr), _len(0) {}

    constexpr Slice(T const *cstr) requires(Meta::Same<T, char>)
        : _buf(cstr), _len(strlen(cstr)) {}

    constexpr Slice(T const *buf, size_t len)
        : _buf(buf), _len(len) {}

    constexpr T const &operator[](size_t i) const {
        return _buf[i];
    }

    constexpr Slice<T> sub(size_t start, size_t end) const {
        return Slice(_buf + start, clamp(end, start, _len));
    }

    constexpr Ordr cmp(Slice<T> const &other) const {
        return Op::cmp(_buf, _len, other._buf, other._len);
    }

    constexpr T const *buf() const {
        return _buf;
    }

    constexpr size_t len() const {
        return _len;
    }

    constexpr auto iter() const {
        return Iter([&, i = 0uz]() mutable -> T * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[i++].unwrap();
        });
    }

    constexpr auto iter_rev() const {
        return Iter([&, i = _len - 1]() mutable -> T const * {
            if (i < 0) {
                return NONE;
            }

            return &_buf[i--].unwrap();
        });
    }
};

template <typename T>
struct MutSlice {
    T *_buf;
    size_t _len;

    constexpr MutSlice()
        : _buf(nullptr), _len(0) {}

    constexpr MutSlice(T const *cstr) requires(Meta::Same<T, char>)
        : _buf(const_cast<T *>(cstr)), _len(strlen(cstr)) {}

    constexpr MutSlice(T *buf, size_t len)
        : _buf(buf), _len(len) {}

    constexpr T &operator[](size_t i) {
        return _buf[i];
    }

    constexpr T const &operator[](size_t i) const {
        return _buf[i];
    }

    constexpr operator Slice<T>() const {
        return Slice<T>(_buf, _len);
    }

    constexpr MutSlice sub(size_t start, size_t end) {
        return MutSlice(_buf + start, clamp(end, start, _len));
    }

    constexpr Slice<T> sub(size_t start, size_t end) const {
        return Slice(_buf + start, clamp(end, start, _len));
    }

    constexpr Ordr cmp(Slice<T> const &other) const {
        return Op::cmp(_buf, _len, other._buf, other._len);
    }

    constexpr T *buf() {
        return _buf;
    }

    constexpr T const *buf() const {
        return _buf;
    }

    constexpr size_t len() const {
        return _len;
    }

    constexpr auto iter() {
        return Iter([&, i = 0uz]() mutable -> T * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[i++].unwrap();
        });
    }

    constexpr auto iter_rev() {
        return Iter([&, i = _len - 1]() mutable -> T const * {
            if (i < 0) {
                return NONE;
            }

            return &_buf[i--].unwrap();
        });
    }
};

} // namespace Karm
