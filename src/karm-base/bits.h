#pragma once

#include "opt.h"
#include "range.h"

namespace Karm {

struct Bits {
    uint8_t *_buf;
    size_t _len;

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

    void set(USizeRange range, bool value) {
        for (size_t i = range.start(); i < range.end(); i++) {
            set(i, value);
        }
    }

    void clear() {
        memset(_buf, 0, _len);
    }

    size_t len() const {
        return _len * 8;
    }

    Opt<USizeRange> alloc(size_t count) {
        size_t start = 0;
        size_t size = 0;

        for (size_t i = 0; i < len(); i++) {
            if (get(i)) {
                start = 0;
                size = 0;
            } else {
                if (size == 0) {
                    start = i;
                }

                size++;

                if (size == count) {
                    USizeRange range{start, start + size};
                    set(range, true);
                    return range;
                }
            }
        }

        return NONE;
    }
};

} // namespace Karm
