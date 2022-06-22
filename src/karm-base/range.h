#pragma once

#include "clamp.h"
#include "cons.h"
#include "iter.h"
#include "vec.h"

namespace Karm {

template <typename T>
struct Range {
    T _start{};
    T _size{};

    constexpr Range() = default;

    constexpr Range(T start, T size)
        : _start(start), _size(size) {
    }

    constexpr T start() const {
        return _start;
    }

    constexpr T end() const {
        return _start + _size;
    }

    constexpr bool empty() const {
        return _size == T{};
    }

    constexpr bool any() const {
        return !empty();
    }

    constexpr T size() const {
        return _size;
    }

    constexpr bool contains(T value) const {
        return start() <= value && value < end();
    }

    constexpr bool contains(Range other) const {
        return start() <= other.start() && other.end() <= end();
    }

    constexpr bool contigous(Range other) const {
        return end() == other.start() || start() == other.end();
    }

    bool collide(Range other) const {
        return start() < other.end() && other.start() < end();
    }

    constexpr bool overlaps(Range other) const {
        return start() < other.end() && other.start() < end();
    }

    constexpr Range merge(Range other) const {
        return fromStartEnd(
            min(start(), other.start()),
            max(end(), other.end()));
    }

    bool touch(Range other) const {
        return contigous(other) || collide(other);
    }

    constexpr Pair<Range>
    substract(Range other) const {
        Range lower = fromStartEnd(
            start(),
            min(end(), other.start()));

        Range upper = fromStartEnd(
            max(start(), other.end()),
            end());

        return {
            lower.valid() ? lower : Range{},
            upper.valid() ? upper : Range{},
        };
    }

    constexpr Ordr cmp(Range other) const {
        return Op::cmp(start(), other.start());
    }

    constexpr auto iter() const {
        return range(start(), end());
    }

    constexpr auto iterRev() const {
        return range(end(), start());
    }

    template <typename U>
    constexpr auto as() {
        return U{start(), size()};
    }
};

template <typename T>
struct RangeAlloc {
    Vec<Range<T>> _ranges;

    Opt<Range<T>> alloc(T size) {
        for (auto &range : _ranges) {
            if (range.size() >= size) {
                Range result = {
                    range.start,
                    range.start + size,
                };
                range.start += size;
                return result;
            }
        }
        return NONE;
    }

    Opt<Range<T>> map(Range<T> range) {
        for (auto &r : _ranges) {
            if (r.contains(range)) {
                auto result = r.substract(range);
                free(result.car);
                free(result.cdr);
                return range;
            }
        }
        return {};
    }

    void free(Range<T> range) {
        for (auto &r : _ranges) {
            if (r.contigous(range)) {
                r = r.merge(range);
                return;
            }
        }
        _ranges.pushBack(range);
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
