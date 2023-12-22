#pragma once

#include <karm-base/endian.h>

#include "base.h"

// https://en.wikipedia.org/wiki/MD5
// https://github.com/Zunawe/md5-c/tree/main

namespace Karm::Crypto {

struct Md5 {
    using Digest = Digest<128, struct Md5>;

    struct Abcd {
        u32 a = 0x67452301;
        u32 b = 0xefcdab89;
        u32 c = 0x98badcfe;
        u32 d = 0x10325476;

        auto operator+=(Abcd const &other) -> Abcd & {
            a += other.a;
            b += other.b;
            c += other.c;
            d += other.d;
            return *this;
        }

        Digest digest() const {
            return {
                u8(a), u8(a >> 8), u8(a >> 16), u8(a >> 24),
                u8(b), u8(b >> 8), u8(b >> 16), u8(b >> 24),
                u8(c), u8(c >> 8), u8(c >> 16), u8(c >> 24),
                u8(d), u8(d >> 8), u8(d >> 16), u8(d >> 24)};
        }
    };

    using Block = Array<u32le, 16>;

    static constexpr Array<u32, 64> S = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    static constexpr Array<u32, 64> K = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

    Abcd _state{};
    usize _len{};

    auto _proc(Abcd abcd, Block const &input) -> Abcd {
        u32 e;
        usize j;

        for (usize i = 0; i < 64; ++i) {
            switch (i / 16) {
            case 0:
                e = (abcd.b & abcd.c) | (~abcd.b & abcd.d);
                j = i;
                break;

            case 1:
                e = (abcd.d & abcd.b) | (~abcd.d & abcd.c);
                j = ((i * 5) + 1) % 16;
                break;

            case 2:
                e = abcd.b ^ abcd.c ^ abcd.d;
                j = ((i * 3) + 5) % 16;
                break;

            default:
                e = abcd.c ^ (abcd.b | ~abcd.d);
                j = (i * 7) % 16;
                break;
            }

            e += abcd.a + K[i] + input[j];
            abcd.a = abcd.d;
            abcd.d = abcd.c;
            abcd.c = abcd.b;
            abcd.b += rol(e, S[i]);
        }

        return abcd;
    }

    usize _step(Bytes bytes) {
        Block block;
        auto blockBytes = block.mutBytes();

        // Fill the buffer
        auto n = copy(bytes, mutSub(blockBytes, _len, 64));
        _len += n;

        // If the buffer is full, process it
        if (_len == 64) {
            _state += _proc(_state, block);
            _len = 0;
        }

        return n;
    }

    void _pad() {
        auto block = Block{};
        auto blockBytes = block.mutBytes();
        blockBytes[0] = 0x80;
        blockBytes[15] = _len * 8;
        add(blockBytes);
    }

    void add(Bytes bytes) {
        while (bytes.len() > 0) {
            auto n = _step(bytes);
            bytes = next(bytes, n);
        }
    }

    void reset() {
        _state = {};
        _len = 0;
    }

    auto sum() const {
        return _state;
    }

    auto digest() {
        _pad();

        // Add the length
        auto len = _len * 8;
        auto block = Block{};
        auto blockBytes = block.mutBytes();
        blockBytes[14] = len >> 32;
        blockBytes[15] = len;
        add(blockBytes);

        // Return the sum
        return _state.digest();
    };
};

} // namespace Karm::Crypto
