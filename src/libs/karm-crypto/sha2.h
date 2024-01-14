#pragma once

#include <karm-base/endian.h>
#include <karm-base/vec.h>

#include "base.h"

namespace Karm::Crypto {

static constexpr Array<u32, 64> SHA256_K = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

struct Sha256 {
    static usize const BLOCK_SIZE = 64;
    static usize const ROUNDS = 64;

    using Digest = Digest<256, struct Sha256>;

    Array<u32be, 8> _state{};
    Array<Byte, BLOCK_SIZE> _buf;
    usize _curr = 0;
    usize _total = 0;

    void reset() {
        _state = {
            0x6a09e667,
            0xbb67ae85,
            0x3c6ef372,
            0xa54ff53a,
            0x510e527f,
            0x9b05688c,
            0x1f83d9ab,
            0x5be0cd19};
    }

    void _proc() {
        auto const SIGN0 = [](u32 x) {
            return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
        };

        auto const SIGN1 = [](u32 x) {
            return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
        };

        auto const CH = [](u32 x, u32 y, u32 z) {
            return (x & y) ^ (~x & z);
        };

        auto const MAJ = [](u32 x, u32 y, u32 z) {
            return (x & y) ^ (x & z) ^ (y & z);
        };

        auto const EP0 = [](u32 x) {
            return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
        };

        auto const EP1 = [](u32 x) {
            return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
        };

        Array<u32, BLOCK_SIZE> m{};

        size_t i = 0;
        for (size_t j = 0; i < 16; ++i, j += 4) {
            m[i] = (_buf[j] << 24) | (_buf[j + 1] << 16) | (_buf[j + 2] << 8) | _buf[j + 3];
        }

        for (; i < BLOCK_SIZE; ++i) {
            m[i] = SIGN1(m[i - 2]) + m[i - 7] + SIGN0(m[i - 15]) + m[i - 16];
        }

        auto a = _state[0], b = _state[1],
             c = _state[2], d = _state[3],
             e = _state[4], f = _state[5],
             g = _state[6], h = _state[7];

        for (i = 0; i < ROUNDS; ++i) {
            auto temp0 = h + EP1(e) + CH(e, f, g) + SHA256_K[i] + m[i];
            auto temp1 = EP0(a) + MAJ(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + temp0;
            d = c;
            c = b;
            b = a;
            a = temp0 + temp1;
        }

        _state[0] = _state[0] + a;
        _state[1] = _state[1] + b;
        _state[2] = _state[2] + c;
        _state[3] = _state[3] + d;
        _state[4] = _state[4] + e;
        _state[5] = _state[5] + f;
        _state[6] = _state[6] + g;
        _state[7] = _state[7] + h;
    }

    usize _step(Bytes bytes) {
        auto n = copy(bytes, mutSub(_buf, _curr, BLOCK_SIZE));
        _curr += n;
        _total += n;
        if (_curr == BLOCK_SIZE) {
            _proc();
            _curr = 0;
        }
        return n;
    }

    void _pad() {
        auto total = _total;
        Byte pad = 0x80;
        _step({&pad, 1});

        Array<u8, 64> zeros{};
        _step(sub(zeros, 0, ((64 - (_total + 1 + 8)) % 64)));

        u32be len = total * 8;
        _step(len.bytes());
    }

    void add(Bytes bytes) {
        while (bytes.len() > 0) {
            auto n = _step(bytes);
            bytes = next(bytes, n);
        }
    }

    auto sum() const {
        return _state;
    }

    Digest digest() {
        _pad();
        return unionCast<Digest>(_state);
    }
};

} // namespace Karm::Crypto
