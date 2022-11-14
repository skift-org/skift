#pragma once

#include "opt.h"
#include "range.h"
#include "slice.h"

namespace Karm {

using BitsRange = Range<size_t, struct BitsRangeTag>;

struct Bits {
    uint8_t *_buf{};
    size_t _len{};

    Bits(MutSlice<uint8_t> slice)
        : _buf{slice.buf()}, _len{slice.len()} {
    }

    bool get(size_t index) const {
        return _buf[index / 8] & (1 << (index % 8));
    }

    void set(size_t index, bool value) {
        if (value) {
            _buf[index / 8] |= (1 << (index % 8));
        } else {
            _buf[index / 8] &= ~(1 << (index % 8));
        }
    }

    void set(BitsRange range, bool value) {
        for (size_t i = range.start; i < range.end(); i++) {
            set(i, value);
        }
    }

    void clear() {
        memset(_buf, 0, _len);
    }

    size_t len() const {
        return _len * 8;
    }

    Opt<BitsRange> alloc(size_t count, size_t start = -1, bool upper = true) {
        start = min(start, len());

        if (_len == 0) {
            return Opt<BitsRange>();
        }

        BitsRange range = {};
        int offset = upper ? -1 : 1;

        for (size_t i = start; (upper ? i > 0 : i < len()); i += offset) {
            if (get(i)) {
                range = {};
            } else {
                if (range.size == 0) {
                    range.start = i;
                }

                range.size++;

                if (range.size == count) {
                    set(range, true);
                    return range;
                }
            }
        }

        return NONE;
    }

    size_t used() const {
        size_t res = 0;

        for (size_t i = 0; i < len(); i++) {
            if (get(i)) {
                res++;
            }
        }

        return res;
    }
};

} // namespace Karm
