#pragma once

#include <karm-base/endian.h>

#include "base.h"

// https://en.wikipedia.org/wiki/Cyclic_redundancy_check

namespace Karm::Crypto {

struct Crc32 {
    using Digest = Digest<32, struct Crc32>;

    u32 _state = 0xffffffff;
    Array<u32, 256> _table{};

    Crc32(u32 poly = 0xedb88320)
        : _table(makeTable(poly)) {}

    static constexpr Array<u32, 256> makeTable(u32 poly) {
        Array<u32, 256> table;
        for (u32 i = 0; i < 256; ++i) {
            u32 c = i;
            for (u32 j = 0; j < 8; ++j) {
                if (c & 1) {
                    c = (c >> 1) ^ poly;
                } else {
                    c >>= 1;
                }
            }
            table[i] = c;
        }
        return table;
    }

    void add(Bytes bytes) {
        for (usize i = 0; i < bytes.len(); ++i) {
            _state = _table[(_state ^ bytes[i]) & 0xff] ^ (_state >> 8);
        }
    }

    void reset() {
        _state = 0xffffffff;
    }

    u32 sum() const {
        return ~_state;
    }

    Digest digest() const {
        return unionCast<Digest, u32be>(sum());
    }
};

} // namespace Karm::Crypto
