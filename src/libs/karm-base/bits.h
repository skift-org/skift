#pragma once

#include "opt.h"
#include "range.h"
#include "slice.h"

namespace Karm {

using BitsRange = Range<usize, struct BitsRangeTag>;

struct Bits {
    u8* _buf{};
    usize _len{};

    Bits(MutSlice<u8> slice)
        : _buf(slice.buf()),
          _len(slice.len()) {}

    bool get(usize index) const {
        return _buf[index / 8] & (1 << (index % 8));
    }

    void set(usize index, bool value) {
        if (value) {
            _buf[index / 8] |= (1 << (index % 8));
        } else {
            _buf[index / 8] &= ~(1 << (index % 8));
        }
    }

    void set(BitsRange range, bool value) {
        for (usize i = range.start; i < range.end(); i++) {
            set(i, value);
        }
    }

    void fill(bool value) {
        ::fill(mutBytes(), value ? 0xff_byte : 0x00_byte);
    }

    usize len() const {
        return _len * 8;
    }

    Opt<BitsRange> alloc(usize count, usize start, bool upper = true) {
        start = min(start, len());

        if (_len == 0) {
            return NONE;
        }

        BitsRange range = {};

        for (usize i = start;
             upper ? i > 0 : i < len();
             i += upper ? -1 : 1) {

            if (get(i)) {
                range = {};
            } else {
                if (range.size == 0 or upper) {
                    range.start = i;
                }

                range.size++;
            }

            if (range.size == count) {
                set(range, true);
                return range;
            }
        }

        return NONE;
    }

    usize used() const {
        usize res = 0;

        for (usize i = 0; i < len(); i++) {
            if (get(i)) {
                res++;
            }
        }

        return res;
    }

    void visit(auto cb) {
        BitsRange range = {};
        for (usize i = 0; i < len(); i++) {
            if (get(i)) {
                if (range.size > 0) {
                    cb(range);
                }

                range = {};
            } else {
                if (range.size == 0) {
                    range.start = i;
                }

                range.size++;
            }
        }
    }

    Bytes bytes() const {
        return Bytes(_buf, _len);
    }

    MutBytes mutBytes() {
        return MutBytes(_buf, _len);
    }
};

} // namespace Karm
