#pragma once

#include "align.h"
#include "clamp.h"
#include "cons.h"
#include "iter.h"
#include "ordr.h"

namespace Karm {

template <typename T, typename TAG = struct _default_range_tag>
struct Range {
    T start{};
    T size{};

    static constexpr Range fromStartEnd(T start, T end) {
        return {start, end - start};
    }

    constexpr Range() = default;

    constexpr Range(T start, T size)
        : start(start), size(size) {
    }

    constexpr T end() const {
        return start + size;
    }

    constexpr bool empty() const {
        return size == T{};
    }

    constexpr bool any() const {
        return not empty();
    }

    constexpr bool valid() const {
        return size >= T{};
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
            max(end(), other.end()));
    }

    constexpr Range halfUnder(Range other) {
        if (overlaps(other) and start < other.start) {
            return {start, other.start - start};
        }

        return {};
    }

    constexpr Range halfOver(Range other) {
        if (overlaps(other) and other.end() < end()) {
            return {other.end(), end() - other.end()};
        }

        return {};
    }

    constexpr Cons<Range> split(Range other) {
        return {halfUnder(other), halfOver(other)};
    }

    constexpr auto iter() const {
        return range(start, end());
    }

    constexpr auto iterRev() const {
        return range(end(), start);
    }

    template <typename U>
    constexpr auto as() {
        return U{start, size};
    }

    constexpr Ordr cmp(Range<T> other) const {
        if (start == other.start and size == other.size)
            return Ordr::EQUAL;

        if (start < other.start)
            return Ordr::LESS;

        if (start > other.start)
            return Ordr::GREATER;
    }

    Res<> ensureAligned(T alignment) const {
        if (not isAlign(start, alignment))
            return Error::invalidInput("start is not aligned");

        if (not isAlign(size, alignment))
            return Error::invalidInput("size is not aligned");

        return Ok();
    }
};

using I8Range = Range<int8_t>;
using I16Range = Range<int16_t>;
using I32Range = Range<int32_t>;
using I64Range = Range<int64_t>;
using ISizeRange = Range<ssize_t>;

using U8Range = Range<uint8_t>;
using U16Range = Range<uint16_t>;
using U32Range = Range<uint32_t>;
using U64Range = Range<uint64_t>;
using USizeRange = Range<size_t>;

} // namespace Karm
