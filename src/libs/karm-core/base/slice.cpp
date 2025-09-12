module;

#include <karm-core/macros.h>

export module Karm.Core:base.slice;

import :base.base;
import :base.hash;
import :base.iter;
import :base.range;

namespace Karm {

#pragma clang unsafe_buffer_usage begin

export template <typename T, typename U = typename T::Inner>
concept Sliceable =
    requires(T const& t) {
        typename T::Inner;
        { t.len() } -> Meta::Same<usize>;
        { t.buf() } -> Meta::Same<U const*>;
        { t[0uz] } -> Meta::Same<U const&>;
    };

export template <typename T, typename U = typename T::Inner>
concept MutSliceable =
    Sliceable<T, U> and
    requires(T& t) {
        { t.len() } -> Meta::Same<usize>;
        { t.buf() } -> Meta::Same<U*>;
        { t[0uz] } -> Meta::Same<U&>;
    };

export template <Sliceable T, Sliceable U>
    requires Meta::Comparable<typename T::Inner, typename U::Inner>
constexpr auto operator<=>(T const& lhs, U const& rhs) -> decltype(lhs[0] <=> rhs[0]) {
    for (usize i = 0; i < min(lhs.len(), rhs.len()); i++) {
        auto result = lhs[i] <=> rhs[i];
        if (result != 0)
            return result;
    }
    return lhs.len() <=> rhs.len();
}

export template <Sliceable T, Sliceable U>
    requires Meta::Equatable<typename T::Inner, typename U::Inner>
constexpr bool operator==(T const& lhs, U const& rhs) {
    if (lhs.len() != rhs.len())
        return false;

    for (usize i = 0; i < min(lhs.len(), rhs.len()); i++) {
        if (lhs[i] != rhs[i])
            return false;
    }

    return true;
}

export template <Sliceable T>
constexpr u64 hash(T const& v)
    requires(not requires(T const t) {
        { t.hash() } -> Meta::Same<u64>;
    })
{
    u64 res = hash();
    for (usize i = 0; i < v.len(); i++)
        res = hash(res, v.buf()[i]);
    return res;
}

export template <typename T>
struct Slice {
    using Inner = T;

    T const* _buf{};
    usize _len{};

    static constexpr Slice fromNullterminated(T const* buf) {
        usize len = 0;
        while (buf[len])
            len++;
        return {buf, len};
    }

    static constexpr Slice fromNullterminated(T const* buf, usize maxLen) {
        usize len = 0;
        while (buf[len] and len < maxLen)
            len++;
        return {buf, len};
    }

    constexpr Slice() = default;

    constexpr Slice(T const* buf, usize len)
        : _buf(buf), _len(len) {}

    constexpr Slice(T const* begin, T const* end)
        : Slice(begin, end - begin) {}

    constexpr Slice(Sliceable<T> auto const& other)
        : Slice(other.buf(), other.len()) {}

    constexpr T const& operator[](usize i) const {
        if (i >= _len) {
            panic("index out of bounds");
        }
        return _buf[i];
    }

    constexpr T const* buf() const { return _buf; }

    constexpr T const* begin() const { return _buf; }

    constexpr T const* end() const { return _buf + _len; }

    constexpr usize len() const { return _len; }

    template <typename U>
    constexpr Slice<U> cast() const {
        return Slice<U>{(U const*)_buf, _len};
    }

    constexpr explicit operator bool() const {
        return _len > 0;
    }
};

export template <typename T>
struct MutSlice {
    using Inner = T;

    T* _buf{};
    usize _len{};

    static constexpr MutSlice fromNullterminated(T* buf) {
        usize len = 0;
        while (buf[len])
            len++;
        return {buf, len};
    }

    constexpr MutSlice() = default;

    constexpr MutSlice(T* buf, usize len)
        : _buf(buf), _len(len) {}

    constexpr MutSlice(T* begin, T* end)
        : MutSlice(begin, end - begin) {}

    constexpr MutSlice(MutSliceable<T> auto& other)
        : MutSlice(other.buf(), other.len()) {}

    constexpr T& operator[](usize i) {
        if (i >= _len) {
            panic("index out of bounds");
        }
        return _buf[i];
    }

    constexpr T const& operator[](usize i) const {
        if (i >= _len) {
            panic("index out of bounds");
        }
        return _buf[i];
    }

    constexpr T* buf() { return _buf; }

    constexpr T const* buf() const { return _buf; }

    constexpr T* begin() { return _buf; }

    constexpr T* end() { return _buf + _len; }

    constexpr T const* begin() const { return _buf; }

    constexpr T const* end() const { return _buf + _len; }

    constexpr usize len() const { return _len; }

    template <typename U>
    constexpr MutSlice<U> cast() const {
        return MutSlice<U>{(U*)_buf, _len * sizeof(T) / sizeof(U)};
    }

    constexpr explicit operator bool() const {
        return _len > 0;
    }
};

export template <typename T>
struct Niche<Slice<T>> {
    struct Content {
        char const* ptr;
        usize _len;

        always_inline constexpr Content() : ptr(NICHE_PTR) {}

        always_inline constexpr bool has() const {
            return ptr != NICHE_PTR;
        }
    };
};

export template <typename T>
struct Niche<MutSlice<T>> {
    struct Content : public Niche<Slice<T>>::Content {};
};

export using Bytes = Slice<u8>;

export using MutBytes = MutSlice<u8>;

export template <Sliceable S, typename T = typename S::Inner>
constexpr Slice<T> sub(S const& slice, usize start, usize end) {
    return {
        slice.buf() + start,
        clamp(end, start, slice.len()) - start,
    };
}

export template <Sliceable S, typename T = typename S::Inner>
constexpr Slice<T> sub(S const& slice, Range<usize> range) {
    return sub(slice, range.start, range.end());
}

export template <Sliceable S, typename T = typename S::Inner>
constexpr Slice<T> sub(S const& slice) {
    return {
        slice.buf(),
        slice.len(),
    };
}

export template <Sliceable S, typename T = typename S::Inner>
Slice<T> next(S const& slice, usize start = 1) {
    return sub(slice, start, slice.len());
}

export template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutSub(S& slice, usize start, usize end) {
    return {
        slice.buf() + start,
        clamp(end, start, slice.len()) - start,
    };
}

export template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutSub(S& slice, urange range) {
    return mutSub(slice, range.start, range.end());
}

export template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutSub(S& slice) {
    return {
        slice.buf(),
        slice.len(),
    };
}

export template <MutSliceable S, typename T = typename S::Inner>
MutSlice<T> mutNext(S& slice, usize start = 1) {
    return mutSub(slice, start, slice.len());
}

export template <Sliceable S>
Bytes bytes(S const& slice) {
    return {
        reinterpret_cast<u8 const*>(slice.buf()),
        slice.len() * sizeof(typename S::Inner),
    };
}

export template <MutSliceable S>
MutBytes mutBytes(S& slice) {
    return {
        reinterpret_cast<u8*>(slice.buf()),
        slice.len() * sizeof(typename S::Inner),
    };
}

export template <Sliceable S>
usize sizeOf(S const& slice) {
    return slice.len() * sizeof(typename S::Inner);
}

export template <Sliceable S>
constexpr auto iter(S const& slice) {
    return Iter([&slice, i = 0uz] mutable -> typename S::Inner const* {
        if (i >= slice.len()) {
            return nullptr;
        }

        return &slice.buf()[i++];
    });
}

export template <Sliceable S>
constexpr auto iterRev(S const& slice) {
    return Iter([&slice, i = slice.len()] mutable -> typename S::Inner const* {
        if (i == 0) {
            return nullptr;
        }

        return &slice.buf()[--i];
    });
}

export template <MutSliceable S>
constexpr auto mutIter(S& slice) {
    usize i = 0uz;
    return Iter([&slice, i] mutable -> typename S::Inner* {
        if (i >= slice.len()) {
            return nullptr;
        }

        return &slice.buf()[i++];
    });
}

export template <MutSliceable S>
constexpr auto mutIterRev(S& slice) {
    usize i = slice.len();
    return Iter([&slice, i] mutable -> typename S::Inner* {
        if (i == 0) {
            return nullptr;
        }

        return &slice.buf()[--i];
    });
}

export template <Sliceable S>
constexpr auto iterSplit(S const& slice, typename S::Inner const& sep) {
    usize i = 0uz;
    return Iter([slice, sep, i] mutable -> Opt<Slice<typename S::Inner>> {
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

export constexpr bool isEmpty(Sliceable auto const& slice) {
    return slice.len() == 0;
}

export constexpr bool any(Sliceable auto const& slice) {
    return slice.len() > 0;
}

export constexpr auto const* begin(Sliceable auto const& slice) {
    return slice.buf();
}

export constexpr auto const* end(Sliceable auto const& slice) {
    return slice.buf() + slice.len();
}

export constexpr auto* begin(MutSliceable auto& slice) {
    return slice.buf();
}

export constexpr auto* end(MutSliceable auto& slice) {
    return slice.buf() + slice.len();
}

export constexpr auto const& first(Sliceable auto const& slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[0];
}

export constexpr auto& first(MutSliceable auto& slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[0];
}

export constexpr auto const& last(Sliceable auto const& slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[slice.len() - 1];
}

export constexpr auto& last(MutSliceable auto& slice) {
    if (isEmpty(slice)) [[unlikely]]
        panic("empty slice");
    return slice.buf()[slice.len() - 1];
}

export template <typename T>
constexpr usize move(MutSlice<T> src, MutSlice<T> dest) {
    usize l = min(src.len(), dest.len());

    for (usize i = 0; i < l; i++) {
        dest[i] = std::move(src[i]);
    }

    return l;
}

export template <typename T>
constexpr usize copy(Slice<T> src, MutSlice<T> dest) {
    usize copied = 0;

    for (usize i = 0; i < min(src.len(), dest.len()); i++) {
        dest[i] = src[i];
        copied++;
    }

    return copied;
}

export template <typename T>
constexpr usize copyWithin(MutSlice<T> slice, urange src, usize dst) {
    usize copied = 0;

    for (usize i = src.start; i < src.end(); i++) {
        slice[dst + copied] = slice[i];
        copied++;
    }

    return copied;
}

export template <typename T>
constexpr void reverse(MutSlice<T> slice) {
    for (usize i = 0; i < slice.len() / 2; i++) {
        std::swap(slice[i], slice[slice.len() - i - 1]);
    }
}

export template <typename T>
constexpr usize fill(MutSlice<T> slice, T value) {
    for (usize i = 0; i < slice.len(); i++) {
        slice[i] = value;
    }
    return slice.len();
}

export template <typename T>
constexpr usize zeroFill(MutSlice<T> slice) {
    return fill(slice, {});
}

export always_inline constexpr void sort(MutSliceable auto& slice, auto cmp) {
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

export always_inline constexpr void sort(MutSliceable auto& slice) {
    sort(slice, [](auto const& a, auto const& b) {
        return a <=> b;
    });
}

export always_inline constexpr void stableSort(MutSliceable auto& slice, auto cmp) {
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

export always_inline constexpr void stableSort(MutSliceable auto& slice) {
    stableSort(slice, [](auto const& a, auto const& b) {
        return a <=> b;
    });
}

export template <Sliceable T, typename U = T::Inner>
always_inline constexpr Opt<usize> indexOf(T const& slice, Meta::Equatable<U> auto const& needle) {
    for (usize i = 0; i < slice.len(); i++)
        if (slice[i] == needle)
            return i;
    return NONE;
}

export template <Sliceable T, typename U = T::Inner>
Generator<Slice<U>> split(T const& slice, Meta::Equatable<U> auto const delim) {
    Slice<U> curr = sub(slice);
    while (curr) {
        auto end = indexOf(curr, delim);
        if (not end)
            break;
        co_yield sub(curr, 0, end.unwrap());
        curr = next(curr, end.unwrap() + 1);
    }
    co_yield curr;
}

export template <Sliceable T, typename U = T::Inner>
always_inline constexpr bool contains(T const& slice, Meta::Equatable<U> auto const& needle) {
    return indexOf(slice, needle).has();
}

export always_inline constexpr bool contains(Sliceable auto const& slice, Sliceable auto const& needle, auto const cmp) {
    if (needle.len() == 0)
        return true;

    if (slice.len() < needle.len())
        return false;

    for (usize i = 0; i < slice.len() - needle.len() + 1; i++) {
        bool found = true;
        for (usize j = 0; j < needle.len(); j++) {
            if (not cmp(slice[i + j], needle[j])) {
                found = false;
                break;
            }
        }
        if (found)
            return true;
    }

    return false;
}

export template <Sliceable T1, Sliceable T2, typename U1 = T1::Inner, typename U2 = T2::Inner>
always_inline constexpr bool contains(T1 const& slice, T2 const& needle)
    requires Meta::Equatable<U1, U2>
{
    return contains(slice, needle, [](U1 const& a, U2 const& b) {
        return a == b;
    });
}

export template <Sliceable T, typename U = T::Inner>
always_inline constexpr Opt<usize> lastIndexOf(T const& slice, Meta::Equatable<U> auto const& needle) {
    for (usize i = slice.len(); i > 0; i--)
        if (slice[i - 1] == needle)
            return i - 1;
    return NONE;
}

export always_inline Opt<usize> search(Sliceable auto const& slice, auto const& cmp) {
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

export always_inline Opt<usize> searchLowerBound(Sliceable auto const& slice, auto const& cmp) {
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

export always_inline Opt<usize> searchUpperBound(Sliceable auto const& slice, auto const& cmp) {
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
export enum struct Match {
    NO,      //< No match.
    PARTIAL, //< The pattern is part of the input.
    YES,     //< The pattern is the input.
};

export always_inline Match startWith(Sliceable auto const& slice, Sliceable auto const& prefix, auto cmp) {
    if (slice.len() < prefix.len())
        return Match::NO;

    for (usize i = 0; i < prefix.len(); i++)
        if (not cmp(slice[i], prefix[i]))
            return Match::NO;

    if (slice.len() != prefix.len())
        return Match::PARTIAL;

    return Match::YES;
}

export template <Sliceable T1, Sliceable T2, typename U1 = T1::Inner, typename U2 = T2::Inner>
    requires Meta::Equatable<U1, U2>
always_inline Match startWith(T1 const& slice, T2 const& prefix) {
    return startWith(slice, prefix, [](U1 const& a, U2 const& b) {
        return a == b;
    });
}

export always_inline Match endWith(Sliceable auto const& slice, Sliceable auto const& suffix, auto cmp) {
    if (slice.len() < suffix.len())
        return Match::NO;

    for (usize i = 0; i < suffix.len(); i++)
        if (not cmp(slice[slice.len() - suffix.len() + i], suffix[i]))
            return Match::NO;

    if (slice.len() != suffix.len())
        return Match::PARTIAL;

    return Match::YES;
}

export template <Sliceable T1, Sliceable T2, typename U1 = T1::Inner, typename U2 = T2::Inner>
    requires Meta::Equatable<U1, U2>
always_inline Match endWith(T1 const& slice, T2 const& suffix) {
    return endWith(slice, suffix, [](U1 const& a, U2 const& b) {
        return a == b;
    });
}

#pragma clang unsafe_buffer_usage end

} // namespace Karm

export constexpr Karm::Bytes operator""_bytes(char const* buf, Karm::usize len) {
    return Karm::Bytes{
        reinterpret_cast<Karm::u8 const*>(buf),
        len,
    };
}
