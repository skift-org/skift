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

    constexpr Slice(T const *begin, T const *end)
        : _buf(begin), _len(end - begin) {}

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

    constexpr char const *begin() const {
        return _buf;
    }

    constexpr char const *end() const {
        return _buf + _len;
    }

    constexpr auto iter() const {
        return Iter([&, i = 0uz]() mutable -> T const * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[i++];
        });
    }

    constexpr auto iterRev() const {
        return Iter([&, i = _len - 1]() mutable -> T const * {
            if (i < 0) {
                return nullptr;
            }

            return &_buf[i--];
        });
    }
};

template <typename T>
struct MutSlice {
    T *_buf;
    size_t _len;

    constexpr MutSlice()
        : _buf(nullptr), _len(0) {}

    constexpr MutSlice(T *begin, T *end)
        : _buf(begin), _len(end - begin) {}

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

    constexpr auto iterRev() {
        return Iter([&, i = _len - 1]() mutable -> T const * {
            if (i < 0) {
                return NONE;
            }

            return &_buf[i--].unwrap();
        });
    }
};

} // namespace Karm
