#pragma once

#include "iter.h"
#include "range.h"

namespace Karm {

using Byte = u8;

inline constexpr Byte operator""_byte(unsigned long long arg) noexcept {
    return static_cast<Byte>(arg);
}

template <typename T, typename U = typename T::Inner>
concept Sliceable =
    requires(T const &t) {
        typename T::Inner;
        { t.len() } -> Meta::Same<usize>;
        { t.buf() } -> Meta::Same<U const *>;
        { t[0uz] } -> Meta::Same<U const &>;
    };

template <typename T, typename U = typename T::Inner>
concept MutSliceable =
    Sliceable<T, U> and
    requires(T &t) {
        { t.len() } -> Meta::Same<usize>;
        { t.buf() } -> Meta::Same<U *>;
        { t[0uz] } -> Meta::Same<U &>;
    };

template <Sliceable T, Sliceable U>
    requires Meta::Comparable<typename T::Inner, typename U::Inner>
constexpr auto operator<=>(T const &lhs, U const &rhs) -> decltype(lhs[0] <=> rhs[0]) {
    for (usize i = 0; i < min(lhs.len(), rhs.len()); i++) {
        auto result = lhs[i] <=> rhs[i];
        if (result != 0)
            return result;
    }
    return lhs.len() <=> rhs.len();
}

template <Sliceable T, Sliceable U>
    requires Meta::Equatable<typename T::Inner, typename U::Inner>
constexpr bool operator==(T const &lhs, U const &rhs) {
    if (lhs.len() != rhs.len())
        return false;

    for (usize i = 0; i < min(lhs.len(), rhs.len()); i++) {
        if (lhs[i] != rhs[i])
            return false;
    }

    return true;
}

template <typename T>
struct Slice {
    using Inner = T;

    T const *_buf{};
    usize _len{};

    static constexpr Slice fromNullterminated(T const *buf) {
        usize len = 0;
        while (buf[len])
            len++;
        return {buf, len};
    }

    static constexpr Slice fromNullterminated(T const *buf, usize maxLen) {
        usize len = 0;
        while (buf[len] and len < maxLen)
            len++;
        return {buf, len};
    }

    constexpr Slice() = default;

    constexpr Slice(T const *buf, usize len)
        : _buf(buf), _len(len) {}

    constexpr Slice(T const *begin, T const *end)
        : Slice(begin, end - begin) {}

    constexpr Slice(Sliceable<T> auto const &other)
        : Slice(other.buf(), other.len()) {}

    constexpr T const &operator[](usize i) const {
        if (i >= _len) {
            panic("index out of bounds");
        }
        return _buf[i];
    }

    constexpr T const *buf() const { return _buf; }

    constexpr T const *begin() const { return _buf; }

    constexpr T const *end() const { return _buf + _len; }

    constexpr usize len() const { return _len; }

    template <typename U>
    constexpr Slice<U> cast() const {
        return Slice<U>{(U const *)_buf, _len};
    }

    constexpr explicit operator bool() const {
        return _len > 0;
    }
};

template <typename T>
struct MutSlice {
    using Inner = T;

    T *_buf{};
    usize _len{};

    static constexpr MutSlice fromNullterminated(T *buf) {
        usize len = 0;
        while (buf[len])
            len++;
        return {buf, len};
    }

    constexpr MutSlice() = default;

    constexpr MutSlice(T *buf, usize len)
        : _buf(buf), _len(len) {}

    constexpr MutSlice(T *begin, T *end)
        : MutSlice(begin, end - begin) {}

    constexpr MutSlice(MutSliceable<T> auto &other)
        : MutSlice(other.buf(), other.len()) {}

    constexpr T &operator[](usize i) {
        if (i >= _len) {
            panic("index out of bounds");
        }
        return _buf[i];
    }

    constexpr T const &operator[](usize i) const {
        if (i >= _len) {
            panic("index out of bounds");
        }
        return _buf[i];
    }

    constexpr T *buf() { return _buf; }

    constexpr T const *buf() const { return _buf; }

    constexpr T *begin() { return _buf; }

    constexpr T *end() { return _buf + _len; }

    constexpr T const *begin() const { return _buf; }

    constexpr T const *end() const { return _buf + _len; }

    constexpr usize len() const { return _len; }

    template <typename U>
    constexpr MutSlice<U> cast() const {
        return MutSlice<U>{(U *)_buf, _len * sizeof(T) / sizeof(U)};
    }

    constexpr explicit operator bool() const {
        return _len > 0;
    }
};

using Bytes = Slice<Byte>;

using MutBytes = MutSlice<Byte>;

template <Sliceable S, typename T = typename S::Inner>
constexpr Slice<T> sub(S const &slice, usize start, usize end) {
    return {
        slice.buf() + start,
        clamp(end, start, slice.len()) - start,
    };
}

template <Sliceable S, typename T = typename S::Inner>
constexpr Slice<T> sub(S const &slice, Range<usize> range) {
    return sub(slice, range.start, range.end());
}

template <Sliceable S, typename T = typename S::Inner>
constexpr Slice<T> sub(S const &slice) {
    return {
        slice.buf(),
        slice.len(),
    };
}

template <Sliceable S, typename T = typename S::Inner>
Slice<T> next(S const &slice, usize start = 1) {
    return sub(slice, start, slice.len());
}

template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutSub(S &slice, usize start, usize end) {
    return {
        slice.buf() + start,
        clamp(end, start, slice.len()) - start,
    };
}

template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutSub(S &slice, urange range) {
    return mutSub(slice, range.start, range.end());
}

template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutSub(S &slice) {
    return {
        slice.buf(),
        slice.len(),
    };
}

template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutNext(S &slice, usize start = 1) {
    return mutSub(slice, start, slice.len());
}

template <Sliceable S>
Bytes bytes(S const &slice) {
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
usize sizeOf(S const &slice) {
    return slice.len() * sizeof(typename S::Inner);
}

template <Sliceable S>
constexpr auto iter(S const &slice) {
    return Iter([&slice, i = 0uz] mutable -> typename S::Inner const * {
        if (i >= slice.len()) {
            return nullptr;
        }

        return &slice.buf()[i++];
    });
}

template <Sliceable S>
constexpr auto iterRev(S const &slice) {
    return Iter([&slice, i = slice.len()] mutable -> typename S::Inner const * {
        if (i == 0) {
            return nullptr;
        }

        return &slice.buf()[--i];
    });
}

template <MutSliceable S>
constexpr auto mutIter(S &slice) {
    return Iter([&slice, i = 0uz] mutable -> typename S::Inner * {
        if (i >= slice.len()) {
            return nullptr;
        }

        return &slice.buf()[i++];
    });
}

template <MutSliceable S>
constexpr auto mutIterRev(S &slice) {
    return Iter([&slice, i = slice.len()] mutable -> typename S::Inner * {
        if (i == 0) {
            return nullptr;
        }

        return &slice.buf()[--i];
    });
}

template <Sliceable S>
constexpr auto iterSplit(S const &slice, typename S::Inner const &sep) {
    return Iter([&slice, sep, i = 0uz] mutable -> Opt<Slice<typename S::Inner>> {
        if (i >= slice.len())
            return NONE;

        usize start = i;
        while (i < slice.len() and slice.buf()[i] != sep) {
            i++;
        }

        usize end = i;
        if (i < slice.len()) {
            i++;
        }

        return Slice{
            slice.buf() + start,
            end - start,
        };
    });
}

constexpr bool isEmpty(Sliceable auto const &slice) {
    return slice.len() == 0;
}

constexpr bool any(Sliceable auto const &slice) {
    return slice.len() > 0;
}

constexpr auto const *begin(Sliceable auto const &slice) {
    return slice.buf();
}

constexpr auto const *end(Sliceable auto const &slice) {
    return slice.buf() + slice.len();
}

constexpr auto *begin(MutSliceable auto &slice) {
    return slice.buf();
}

constexpr auto *end(MutSliceable auto &slice) {
    return slice.buf() + slice.len();
}

constexpr auto const &first(Sliceable auto const &slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[0];
}

constexpr auto &first(MutSliceable auto &slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[0];
}

constexpr auto const &last(Sliceable auto const &slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[slice.len() - 1];
}

constexpr auto &last(MutSliceable auto &slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[slice.len() - 1];
}

template <typename T>
constexpr usize move(MutSlice<T> src, MutSlice<T> dest) {
    usize l = min(src.len(), dest.len());

    for (usize i = 0; i < l; i++) {
        dest[i] = std::move(src[i]);
    }

    return l;
}

template <typename T>
constexpr usize copy(Slice<T> src, MutSlice<T> dest) {
    usize copied = 0;

    for (usize i = 0; i < min(src.len(), dest.len()); i++) {
        dest[i] = src[i];
        copied++;
    }

    return copied;
}

template <typename T>
constexpr usize copyWithin(MutSlice<T> slice, urange src, usize dst) {
    usize copied = 0;

    for (usize i = src.start; i < src.end(); i++) {
        slice[dst + copied] = slice[i];
        copied++;
    }

    return copied;
}

template <typename T>
constexpr void reverse(MutSlice<T> slice) {
    for (usize i = 0; i < slice.len() / 2; i++) {
        std::swap(slice[i], slice[slice.len() - i - 1]);
    }
}

template <typename T>
constexpr usize fill(MutSlice<T> slice, T value) {
    for (usize i = 0; i < slice.len(); i++) {
        slice[i] = value;
    }
    return slice.len();
}

template <typename T>
constexpr usize zeroFill(MutSlice<T> slice) {
    return fill(slice, {});
}

always_inline constexpr void sort(MutSliceable auto &slice, auto cmp) {
    if (slice.len() <= 1)
        return;

    auto pivot = slice[slice.len() / 2];
    auto left = 0uz;
    auto right = slice.len() - 1;

    while (left <= right) {
        while (cmp(slice[left], pivot) < 0) {
            left++;
        }

        while (cmp(slice[right], pivot) > 0) {
            right--;
        }

        if (left <= right) {
            std::swap(slice[left], slice[right]);
            left++;
            right--;
        }
    }

    auto rightSlice = mutSub(slice, 0, right + 1);
    sort(rightSlice, cmp);

    auto leftSlice = mutSub(slice, left, slice.len());
    sort(leftSlice, cmp);
}

always_inline constexpr void sort(MutSliceable auto &slice) {
    sort(slice, [](auto const &a, auto const &b) {
        return a <=> b;
    });
}

always_inline constexpr void stableSort(MutSliceable auto &slice, auto cmp) {
    for (usize i = 1; i < slice.len(); i++) {
        auto key = slice[i];
        isize j = i - 1;

        while (j >= 0 and cmp(slice[j], key) > 0) {
            slice[j + 1] = slice[j];
            j--;
        }

        slice[j + 1] = key;
    }
}

always_inline constexpr void stableSort(MutSliceable auto &slice) {
    stableSort(slice, [](auto const &a, auto const &b) {
        return a <=> b;
    });
}

template <Sliceable T, typename U = T::Inner>
always_inline constexpr Opt<usize> indexOf(T const &slice, U const &needle) {
    for (usize i = 0; i < slice.len(); i++)
        if (slice[i] == needle)
            return i;
    return NONE;
}

template <Sliceable T, typename U = T::Inner>
always_inline constexpr bool contains(T const &slice, U const &needle) {
    return indexOf(slice, needle).has();
}

template <Sliceable T, typename U = T::Inner>
always_inline constexpr Opt<usize> lastIndexOf(T const &slice, U const &needle) {
    for (usize i = slice.len(); i > 0; i--)
        if (slice[i - 1] == needle)
            return i - 1;
    return NONE;
}

always_inline Opt<usize> search(Sliceable auto const &slice, auto const &cmp) {
    if (slice.len() == 0)
        return NONE;

    if (cmp(slice[0]) > 0)
        return NONE;

    if (cmp(slice[slice.len() - 1]) < 0)
        return NONE;

    usize left = 0;
    usize right = slice.len() - 1;

    while (left <= right) {
        usize mid = (left + right) / 2;

        auto result = cmp(slice[mid]);

        if (result == 0)
            return mid;

        if (result < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return NONE;
}

always_inline Opt<usize> searchLowerBound(Sliceable auto const &slice, auto const &cmp) {
    if (slice.len() == 0)
        return NONE;

    if (cmp(slice[0]) > 0)
        return NONE;

    if (cmp(slice[slice.len() - 1]) < 0)
        return slice.len() - 1;

    usize left = 0;
    usize right = slice.len() - 1;

    while (left <= right) {
        usize mid = (left + right) / 2;

        auto result = cmp(slice[mid]);

        if (result == 0)
            return mid;

        if (result < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return left - 1;
}

always_inline Opt<usize> searchUpperBound(Sliceable auto const &slice, auto const &cmp) {
    if (slice.len() == 0)
        return NONE;

    if (cmp(slice[0]) >= 0)
        return 0;

    if (cmp(slice[slice.len() - 1]) < 0)
        return NONE;

    usize left = 0;
    usize right = slice.len() - 1;

    while (left <= right) {
        usize mid = (left + right) / 2;

        auto result = cmp(slice[mid]);

        if (result == 0)
            return mid;

        if (result <= 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return left;
}

/// Enumerates the possible results of a match.
enum struct Match {
    NO,      //< No match.
    PARTIAL, //< The pattern is part of the input.
    YES,     //< The pattern is the input.
};

always_inline Match startWith(Sliceable auto const &slice, Sliceable auto const &prefix) {
    if (slice.len() < prefix.len())
        return Match::NO;

    for (usize i = 0; i < prefix.len(); i++)
        if (slice[i] != prefix[i])
            return Match::NO;

    if (slice.len() != prefix.len())
        return Match::PARTIAL;

    return Match::YES;
}

always_inline Match endWith(Sliceable auto const &slice, Sliceable auto const &suffix) {
    if (slice.len() < suffix.len())
        return Match::NO;

    for (usize i = 0; i < suffix.len(); i++)
        if (slice[slice.len() - suffix.len() + i] != suffix[i])
            return Match::NO;

    if (slice.len() != suffix.len())
        return Match::PARTIAL;

    return Match::YES;
}

} // namespace Karm
