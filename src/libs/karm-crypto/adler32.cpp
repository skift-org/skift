export module Karm.Crypto:adler32;

import Karm.Core;

namespace Karm::Crypto {

static constexpr usize ADLER32_BASE = 65521;
static constexpr usize ADLER32_NMAX = 5552;

export u32 adler32(Bytes bytes) {
    auto [buf, len] = bytes;

    u32 s1 = 1;
    u32 s2 = 0;

    while (len > 0) {
        usize k = len < ADLER32_NMAX ? len : ADLER32_NMAX;

        for (usize i = k / 16; i; --i, buf += 16) {
            s1 += buf[0];
            s2 += s1;
            s1 += buf[1];
            s2 += s1;
            s1 += buf[2];
            s2 += s1;
            s1 += buf[3];
            s2 += s1;
            s1 += buf[4];
            s2 += s1;
            s1 += buf[5];
            s2 += s1;
            s1 += buf[6];
            s2 += s1;
            s1 += buf[7];
            s2 += s1;

            s1 += buf[8];
            s2 += s1;
            s1 += buf[9];
            s2 += s1;
            s1 += buf[10];
            s2 += s1;
            s1 += buf[11];
            s2 += s1;
            s1 += buf[12];
            s2 += s1;
            s1 += buf[13];
            s2 += s1;
            s1 += buf[14];
            s2 += s1;
            s1 += buf[15];
            s2 += s1;
        }

        for (usize i = k % 16; i; --i) {
            s1 += *buf++;
            s2 += s1;
        }

        s1 %= ADLER32_BASE;
        s2 %= ADLER32_BASE;

        len -= k;
    }

    return (s2 << 16) | s1;
}

} // namespace Karm::Crypto
