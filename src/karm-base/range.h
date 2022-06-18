#pragma once

#include "clamp.h"
#include "cons.h"
#include "iter.h"
#include "vec.h"

namespace Karm {

template <typename T>
struct Range {
    T start{};
    T end{};

    Range() = default;

    Range(T start, T end)
        : start(start), end(end) {
        if (end < start) {
            panic("Range: end < start");
        }
    }

    bool valid() const {
        return start <= end;
    }

    bool empty() const {
        return start == end;
    }

    bool any() const {
        return !empty();
    }

    T size() const {
        return end - start;
    }

    bool contains(T value) const {
        return start <= value && value < end;
    }

    bool contains(Range other) const {
        return start <= other.start && other.end <= end;
    }

    bool contigous(Range other) const {
        return end == other.start || start == other.end;
    }

    bool overlaps(Range other) const {
        return start < other.end && other.start < end;
    }

    Range merge(Range other) const {
        return {
            min(start, other.start),
            max(end, other.end),
        };
    }

    Pair<Range> substract(Range other) const {
        Range lower = {
            start,
            min(end, other.start),
        };

        Range upper = {
            max(start, other.end),
            end,
        };

        return {
            lower.valid() ? lower : Range{},
            upper.valid() ? upper : Range{},
        };
    }

    Ordr cmp(Range other) const {
        return Op::cmp(start, other.start);
    }

    auto iter() {
        return range(start, end);
    }

    auto iter() const {
        return range(start, end);
    }

    auto iterRev() {
        return range(end, start);
    }

    auto iterRev() const {
        return range(end, start);
    }

    template <typename U>
    auto as() {
        return U{start, end};
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
