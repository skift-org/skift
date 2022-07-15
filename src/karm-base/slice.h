#pragma once

#include "iter.h"
#include "ordr.h"

namespace Karm {

using Byte = uint8_t;

template <typename T, typename U = typename T::Inner>
concept Sliceable = requires(T const &t) {
    typename T::Inner;
    { t.len() } -> Meta::Same<size_t>;
    { t.buf() } -> Meta::Same<U const *>;
    { t[0uz] } -> Meta::Same<U const &>;
};

template <typename T, typename U = typename T::Inner>
concept MutSliceable = Sliceable<T, U> && requires(T &t) {
    { t.len() } -> Meta::Same<size_t>;
    { t.buf() } -> Meta::Same<U *>;
    { t[0uz] } -> Meta::Same<U &>;
};

template <typename T>
struct Slice {
    using Inner = T;

    T const *_buf{};
    size_t _len{};

    constexpr Slice() = default;

    constexpr Slice(T const *buf, size_t len)
        : _buf(buf), _len(len) {}

    constexpr Slice(T const *begin, T const *end)
        : Slice(begin, end - begin) {}

    constexpr Slice(Sliceable<T> auto &other)
        : Slice(other.buf(), other.len()) {}

    constexpr T const &operator[](size_t i) const { return _buf[i]; }

    constexpr T const *buf() const { return _buf; }

    constexpr T const *begin() const { return _buf; }

    constexpr T const *end() const { return _buf + _len; }

    constexpr size_t len() const { return _len; }
};

template <typename T>
struct MutSlice {
    using Inner = T;

    T *_buf{};
    size_t _len{};

    constexpr MutSlice() = default;

    constexpr MutSlice(T *buf, size_t len)
        : _buf(buf), _len(len) {}

    constexpr MutSlice(T *begin, T *end)
        : MutSlice(begin, end - begin) {}

    constexpr MutSlice(MutSliceable<T> auto &other)
        : MutSlice(other.buf(), other.len()) {}

    constexpr T &operator[](size_t i) { return _buf[i]; }

    constexpr T const &operator[](size_t i) const { return _buf[i]; }

    constexpr T *buf() { return _buf; }

    constexpr T const *buf() const { return _buf; }

    constexpr T *begin() { return _buf; }

    constexpr T *end() { return _buf + _len; }

    constexpr T const *begin() const { return _buf; }

    constexpr T const *end() const { return _buf + _len; }

    constexpr size_t len() const { return _len; }
};

using Bytes = Slice<Byte>;

using MutBytes = MutSlice<Byte>;

template <Sliceable S, typename T = typename S::Inner>
Slice<T> sub(S &slice, size_t start, size_t end) {
    return {
        slice.buf() + start,
        clamp(end, start, slice.len()) - start,
    };
}

template <Sliceable S, typename T = typename S::Inner>
Slice<T> next(S &slice, size_t start) {
    return sub(slice, start, slice.len());
}

template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutSub(S &slice, size_t start, size_t end) {
    return {
        slice.buf() + start,
        clamp(end, start, slice.len()) - start,
    };
}

template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutNext(S &slice, size_t start) {
    return mutSub(slice, start, slice.len());
}

template <Sliceable S>
Bytes bytes(S &slice) {
    return {
        reinterpret_cast<Byte const *>(slice.buf()),
        slice.len() * sizeof(typename S::Inner),
    };
}

template <MutSliceable S>
MutBytes mutBytes(S &slice) {
    return {
        reinterpret_cast<Byte *>(slice.buf()),
        slice.len() * sizeof(typename S::Inner),
    };
}

template <Sliceable S>
size_t sizeOf(S &slice) {
    return slice.len() * sizeof(typename S::Inner);
}

template <Sliceable S>
constexpr Ordr cmp(S const &lhs, S const &rhs) {
    return cmp(lhs.buf(), lhs.len(), rhs.buf(), rhs.len());
}

template <Sliceable S>
constexpr auto iter(S &slice) {
    return Iter([&slice, i = 0uz]() mutable -> typename S::Inner const * {
        if (i >= slice.len()) {
            return nullptr;
        }

        return &slice.buf()[i++];
    });
}

template <Sliceable S>
constexpr auto iterRev(S &slice) {
    return Iter([slice, i = slice.len() - 1]() mutable -> typename S::Inner const * {
        if (i < 0) {
            return nullptr;
        }

        return &slice.buf()[i--];
    });
}

template <MutSliceable S>
constexpr auto mutIter(S &slice) {
    return Iter([slice, i = 0uz]() mutable -> typename S::Inner * {
        if (i >= slice.len()) {
            return nullptr;
        }

        return &slice.buf()[i++];
    });
}

template <MutSliceable S>
constexpr auto mutIterRev(S &slice) {
    return Iter([slice, i = slice.len() - 1]() mutable -> typename S::Inner * {
        if (i < 0) {
            return nullptr;
        }

        return &slice.buf()[i--];
    });
}

constexpr size_t len(Sliceable auto &slice) {
    return slice.len();
}

constexpr bool isEmpty(Sliceable auto &slice) {
    return slice.len() == 0;
}

template <Sliceable S>
constexpr auto const *begin(S &slice) {
    return slice.buf();
}

template <Sliceable S>
constexpr auto const *end(S &slice) {
    return slice.buf() + slice.len();
}

template <MutSliceable S>
constexpr auto *begin(S &slice) {
    return slice.buf();
}

template <MutSliceable S>
constexpr auto *end(S &slice) {
    return slice.buf() + slice.len();
}

constexpr auto const &first(Sliceable auto &slice) {
    return slice.buf()[0];
}

constexpr auto &first(MutSliceable auto &slice) {
    return slice.buf()[0];
}

constexpr auto const &last(Sliceable auto &slice) {
    return slice.buf()[slice.len() - 1];
}

constexpr auto &last(MutSliceable auto &slice) {
    return slice.buf()[slice.len() - 1];
}

constexpr auto const &at(Sliceable auto &slice, size_t i) {
    return slice.buf()[i];
}

constexpr auto &at(MutSliceable auto &slice, size_t i) {
    return slice.buf()[i];
}

template <typename T>
constexpr size_t move(MutSlice<T> src, MutSlice<T> dest) {
    size_t l = min(len(src), len(dest));

    for (size_t i = 0; i < l; i++) {
        at(dest, i) = std::move(at(src, i));
    }

    return l;
}

template <typename T>
constexpr size_t copy(Slice<T> src, MutSlice<T> dest) {
    size_t copied = 0;

    for (auto &elem : iter(src)) {
        at(dest, copied++) = elem;
    }

    return copied;
}

template <typename T>
constexpr void reverse(MutSlice<T> slice) {
    for (size_t i = 0; i < len(slice) / 2; i++) {
        std::swap(at(slice, i), at(slice, len(slice) - i - 1));
    }
}

template <typename T>
constexpr size_t fill(MutSlice<T> slice, T value) {
    for (size_t i = 0; i < len(slice); i++) {
        at(slice, i) = value;
    }

    return len(slice);
}

template <typename T>
constexpr size_t zeroFill(MutSlice<T> slice) {
    return fill(slice, {});
}

} // namespace Karm
