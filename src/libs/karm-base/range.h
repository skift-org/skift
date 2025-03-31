#pragma once

#include "align.h"
#include "clamp.h"
#include "tuple.h"

namespace Karm {

template <typename T, typename TAG = struct _RangeTag>
struct Range {
    using Size = decltype(std::declval<T>() - std::declval<T>());

    T start{};
    Size size{};

    static constexpr Range fromStartEnd(T a, T b) {
        if (a < b)
            return {a, b - a};
        return {b, a - b};
    }

    constexpr Range() = default;

    explicit constexpr Range(T start)
        : start(start), size(0) {
    }

    constexpr Range(T start, Size size)
        : start(start), size(size) {
    }

    constexpr T end() const {
        return start + size;
    }

    constexpr void end(T value) {
        size = value - start;
    }

    constexpr bool empty() const {
        return size == Size{};
    }

    constexpr bool any() const {
        return not empty();
    }

    constexpr bool valid() const {
        return size >= Size{};
    }

    constexpr bool contains(T value) const {
        return start <= value and value < end();
    }

    constexpr bool contains(Range other) const {
        return start <= other.start and other.end() <= end();
    }

    constexpr bool contigous(Range other) const {
        return end() == other.start or start == other.end();
    }

    constexpr bool overlaps(Range other) const {
        return start < other.end() and other.start < end();
    }

    constexpr Range merge(Range other) const {
        return fromStartEnd(
            min(start, other.start),
            max(end(), other.end())
        );
    }

    constexpr Range halfUnder(Range other) {
        if (overlaps(other) and start < other.start) {
            return {start, other.start - start};
        }

        return {};
    }

    constexpr Range slice(Size off, Size size) const {
        return {start + off, size};
    }

    constexpr Range slice(Size off) const {
        return slice(off, size - off);
    }

    constexpr Range slice(Range other) const {
        return slice(other.start, other.size);
    }

    constexpr Range halfOver(Range other) {
        if (overlaps(other) and other.end() < end()) {
            return {other.end(), end() - other.end()};
        }

        return {};
    }

    constexpr Pair<Range> split(Range other) {
        return {halfUnder(other), halfOver(other)};
    }

    constexpr auto iter() const {
        return range(start, end());
    }

    constexpr auto iterRev() const {
        return range(end(), start);
    }

    template <typename U>
    constexpr auto cast() const {
        return Range<U, TAG>{static_cast<U>(start), static_cast<U>(size)};
    }

    template <typename U>
    constexpr auto into() const {
        return U{start, size};
    }

    Opt<bool> ensureAligned(T alignment) const {
        if (not isAlign(start, alignment))
            return NONE;

        if (not isAlign(size, alignment))
            return NONE;

        return true;
    }

    std::strong_ordering operator<=>(Range const& other) const {
        if (start == other.start and size == other.size)
            return std::strong_ordering::equal;

        if (start < other.start)
            return std::strong_ordering::less;

        return std::strong_ordering::greater;
    }

    bool operator==(Range const& other) const {
        return start == other.start and size == other.size;
    }

    explicit operator bool() const {
        return any();
    }
};

using irange = Range<isize>;
using urange = Range<usize>;
using frange = Range<f64>;

} // namespace Karm
