#pragma once

#include <karm-base/endian.h>

#include "base.h"

// https://en.wikipedia.org/wiki/Adler-32

namespace Karm::Crypto {

struct Adler32 {
    using Digest = Digest<32, struct Adler32>;

    u32 _state = 1;

    void add(Bytes bytes) {
        u32 s1 = _state & 0xffff;
        u32 s2 = _state >> 16;

        for (usize i = 0; i < bytes.len(); ++i) {
            s1 = (s1 + bytes[i]) % 65521;
            s2 = (s2 + s1) % 65521;
        }

        _state = (s2 << 16) + s1;
    }

    void reset() {
        _state = 1;
    }

    u32 sum() const {
        return _state;
    }

    Digest digest() const {
        return unionCast<Digest, u32be>(_state);
    }
};

} // namespace Karm::Crypto
