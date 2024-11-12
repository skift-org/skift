#include <karm-math/funcs.h>

#include "base.h"

namespace Jpeg {

// MARK: Discrete Cosine Transform ---------------------------------------------

static f32 const m0 = 2.0 * Math::cos(1.0 / 16.0 * 2.0 * Math::PI);
static f32 const m1 = 2.0 * Math::cos(2.0 / 16.0 * 2.0 * Math::PI);
static f32 const m3 = 2.0 * Math::cos(2.0 / 16.0 * 2.0 * Math::PI);
static f32 const m5 = 2.0 * Math::cos(3.0 / 16.0 * 2.0 * Math::PI);
static f32 const m2 = m0 - m5;
static f32 const m4 = m0 + m5;

static f32 const s0 = Math::cos(0.0 / 16.0 * Math::PI) / Math::sqrt(8.0);
static f32 const s1 = Math::cos(1.0 / 16.0 * Math::PI) / 2.0;
static f32 const s2 = Math::cos(2.0 / 16.0 * Math::PI) / 2.0;
static f32 const s3 = Math::cos(3.0 / 16.0 * Math::PI) / 2.0;
static f32 const s4 = Math::cos(4.0 / 16.0 * Math::PI) / 2.0;
static f32 const s5 = Math::cos(5.0 / 16.0 * Math::PI) / 2.0;
static f32 const s6 = Math::cos(6.0 / 16.0 * Math::PI) / 2.0;
static f32 const s7 = Math::cos(7.0 / 16.0 * Math::PI) / 2.0;

void idct(Mcu &mcu) {
    for (usize i = 0; i < 8; ++i) {
        f32 g0 = mcu[0 * 8 + i] * s0;
        f32 g1 = mcu[4 * 8 + i] * s4;
        f32 g2 = mcu[2 * 8 + i] * s2;
        f32 g3 = mcu[6 * 8 + i] * s6;
        f32 g4 = mcu[5 * 8 + i] * s5;
        f32 g5 = mcu[1 * 8 + i] * s1;
        f32 g6 = mcu[7 * 8 + i] * s7;
        f32 g7 = mcu[3 * 8 + i] * s3;

        f32 f0 = g0;
        f32 f1 = g1;
        f32 f2 = g2;
        f32 f3 = g3;
        f32 f4 = g4 - g7;
        f32 f5 = g5 + g6;
        f32 f6 = g5 - g6;
        f32 f7 = g4 + g7;

        f32 e0 = f0;
        f32 e1 = f1;
        f32 e2 = f2 - f3;
        f32 e3 = f2 + f3;
        f32 e4 = f4;
        f32 e5 = f5 - f7;
        f32 e6 = f6;
        f32 e7 = f5 + f7;
        f32 e8 = f4 + f6;

        f32 d0 = e0;
        f32 d1 = e1;
        f32 d2 = e2 * m1;
        f32 d3 = e3;
        f32 d4 = e4 * m2;
        f32 d5 = e5 * m3;
        f32 d6 = e6 * m4;
        f32 d7 = e7;
        f32 d8 = e8 * m5;

        f32 c0 = d0 + d1;
        f32 c1 = d0 - d1;
        f32 c2 = d2 - d3;
        f32 c3 = d3;
        f32 c4 = d4 + d8;
        f32 c5 = d5 + d7;
        f32 c6 = d6 - d8;
        f32 c7 = d7;
        f32 c8 = c5 - c6;

        f32 b0 = c0 + c3;
        f32 b1 = c1 + c2;
        f32 b2 = c1 - c2;
        f32 b3 = c0 - c3;
        f32 b4 = c4 - c8;
        f32 b5 = c8;
        f32 b6 = c6 - c7;
        f32 b7 = c7;

        mcu[0 * 8 + i] = b0 + b7;
        mcu[1 * 8 + i] = b1 + b6;
        mcu[2 * 8 + i] = b2 + b5;
        mcu[3 * 8 + i] = b3 + b4;
        mcu[4 * 8 + i] = b3 - b4;
        mcu[5 * 8 + i] = b2 - b5;
        mcu[6 * 8 + i] = b1 - b6;
        mcu[7 * 8 + i] = b0 - b7;
    }

    for (usize i = 0; i < 8; ++i) {
        f32 g0 = mcu[i * 8 + 0] * s0;
        f32 g1 = mcu[i * 8 + 4] * s4;
        f32 g2 = mcu[i * 8 + 2] * s2;
        f32 g3 = mcu[i * 8 + 6] * s6;
        f32 g4 = mcu[i * 8 + 5] * s5;
        f32 g5 = mcu[i * 8 + 1] * s1;
        f32 g6 = mcu[i * 8 + 7] * s7;
        f32 g7 = mcu[i * 8 + 3] * s3;

        f32 f0 = g0;
        f32 f1 = g1;
        f32 f2 = g2;
        f32 f3 = g3;
        f32 f4 = g4 - g7;
        f32 f5 = g5 + g6;
        f32 f6 = g5 - g6;
        f32 f7 = g4 + g7;

        f32 e0 = f0;
        f32 e1 = f1;
        f32 e2 = f2 - f3;
        f32 e3 = f2 + f3;
        f32 e4 = f4;
        f32 e5 = f5 - f7;
        f32 e6 = f6;
        f32 e7 = f5 + f7;
        f32 e8 = f4 + f6;

        f32 d0 = e0;
        f32 d1 = e1;
        f32 d2 = e2 * m1;
        f32 d3 = e3;
        f32 d4 = e4 * m2;
        f32 d5 = e5 * m3;
        f32 d6 = e6 * m4;
        f32 d7 = e7;
        f32 d8 = e8 * m5;

        f32 c0 = d0 + d1;
        f32 c1 = d0 - d1;
        f32 c2 = d2 - d3;
        f32 c3 = d3;
        f32 c4 = d4 + d8;
        f32 c5 = d5 + d7;
        f32 c6 = d6 - d8;
        f32 c7 = d7;
        f32 c8 = c5 - c6;

        f32 b0 = c0 + c3;
        f32 b1 = c1 + c2;
        f32 b2 = c1 - c2;
        f32 b3 = c0 - c3;
        f32 b4 = c4 - c8;
        f32 b5 = c8;
        f32 b6 = c6 - c7;
        f32 b7 = c7;

        mcu[i * 8 + 0] = b0 + b7;
        mcu[i * 8 + 1] = b1 + b6;
        mcu[i * 8 + 2] = b2 + b5;
        mcu[i * 8 + 3] = b3 + b4;
        mcu[i * 8 + 4] = b3 - b4;
        mcu[i * 8 + 5] = b2 - b5;
        mcu[i * 8 + 6] = b1 - b6;
        mcu[i * 8 + 7] = b0 - b7;
    }
}

void fdtc(Mcu &mcu) {
    for (usize i = 0; i < 8; ++i) {
        f32 a0 = mcu[0 * 8 + i];
        f32 a1 = mcu[1 * 8 + i];
        f32 a2 = mcu[2 * 8 + i];
        f32 a3 = mcu[3 * 8 + i];
        f32 a4 = mcu[4 * 8 + i];
        f32 a5 = mcu[5 * 8 + i];
        f32 a6 = mcu[6 * 8 + i];
        f32 a7 = mcu[7 * 8 + i];

        f32 b0 = a0 + a7;
        f32 b1 = a1 + a6;
        f32 b2 = a2 + a5;
        f32 b3 = a3 + a4;
        f32 b4 = a3 - a4;
        f32 b5 = a2 - a5;
        f32 b6 = a1 - a6;
        f32 b7 = a0 - a7;

        f32 c0 = b0 + b3;
        f32 c1 = b1 + b2;
        f32 c2 = b1 - b2;
        f32 c3 = b0 - b3;
        f32 c4 = b4;
        f32 c5 = b5 - b4;
        f32 c6 = b6 - c5;
        f32 c7 = b7 - b6;

        f32 d0 = c0 + c1;
        f32 d1 = c0 - c1;
        f32 d2 = c2;
        f32 d3 = c3 - c2;
        f32 d4 = c4;
        f32 d5 = c5;
        f32 d6 = c6;
        f32 d7 = c5 + c7;
        f32 d8 = c4 - c6;

        f32 e0 = d0;
        f32 e1 = d1;
        f32 e2 = d2 * m1;
        f32 e3 = d3;
        f32 e4 = d4 * m2;
        f32 e5 = d5 * m3;
        f32 e6 = d6 * m4;
        f32 e7 = d7;
        f32 e8 = d8 * m5;

        f32 f0 = e0;
        f32 f1 = e1;
        f32 f2 = e2 + e3;
        f32 f3 = e3 - e2;
        f32 f4 = e4 + e8;
        f32 f5 = e5 + e7;
        f32 f6 = e6 + e8;
        f32 f7 = e7 - e5;

        f32 g0 = f0;
        f32 g1 = f1;
        f32 g2 = f2;
        f32 g3 = f3;
        f32 g4 = f4 + f7;
        f32 g5 = f5 + f6;
        f32 g6 = f5 - f6;
        f32 g7 = f7 - f4;

        mcu[0 * 8 + i] = g0 * s0;
        mcu[4 * 8 + i] = g1 * s4;
        mcu[2 * 8 + i] = g2 * s2;
        mcu[6 * 8 + i] = g3 * s6;
        mcu[5 * 8 + i] = g4 * s5;
        mcu[1 * 8 + i] = g5 * s1;
        mcu[7 * 8 + i] = g6 * s7;
        mcu[3 * 8 + i] = g7 * s3;
    }

    for (usize i = 0; i < 8; ++i) {
        f32 a0 = mcu[i * 8 + 0];
        f32 a1 = mcu[i * 8 + 1];
        f32 a2 = mcu[i * 8 + 2];
        f32 a3 = mcu[i * 8 + 3];
        f32 a4 = mcu[i * 8 + 4];
        f32 a5 = mcu[i * 8 + 5];
        f32 a6 = mcu[i * 8 + 6];
        f32 a7 = mcu[i * 8 + 7];

        f32 b0 = a0 + a7;
        f32 b1 = a1 + a6;
        f32 b2 = a2 + a5;
        f32 b3 = a3 + a4;
        f32 b4 = a3 - a4;
        f32 b5 = a2 - a5;
        f32 b6 = a1 - a6;
        f32 b7 = a0 - a7;

        f32 c0 = b0 + b3;
        f32 c1 = b1 + b2;
        f32 c2 = b1 - b2;
        f32 c3 = b0 - b3;
        f32 c4 = b4;
        f32 c5 = b5 - b4;
        f32 c6 = b6 - c5;
        f32 c7 = b7 - b6;

        f32 d0 = c0 + c1;
        f32 d1 = c0 - c1;
        f32 d2 = c2;
        f32 d3 = c3 - c2;
        f32 d4 = c4;
        f32 d5 = c5;
        f32 d6 = c6;
        f32 d7 = c5 + c7;
        f32 d8 = c4 - c6;

        f32 e0 = d0;
        f32 e1 = d1;
        f32 e2 = d2 * m1;
        f32 e3 = d3;
        f32 e4 = d4 * m2;
        f32 e5 = d5 * m3;
        f32 e6 = d6 * m4;
        f32 e7 = d7;
        f32 e8 = d8 * m5;

        f32 f0 = e0;
        f32 f1 = e1;
        f32 f2 = e2 + e3;
        f32 f3 = e3 - e2;
        f32 f4 = e4 + e8;
        f32 f5 = e5 + e7;
        f32 f6 = e6 + e8;
        f32 f7 = e7 - e5;

        f32 g0 = f0;
        f32 g1 = f1;
        f32 g2 = f2;
        f32 g3 = f3;
        f32 g4 = f4 + f7;
        f32 g5 = f5 + f6;
        f32 g6 = f5 - f6;
        f32 g7 = f7 - f4;

        mcu[i * 8 + 0] = g0 * s0;
        mcu[i * 8 + 4] = g1 * s4;
        mcu[i * 8 + 2] = g2 * s2;
        mcu[i * 8 + 6] = g3 * s6;
        mcu[i * 8 + 5] = g4 * s5;
        mcu[i * 8 + 1] = g5 * s1;
        mcu[i * 8 + 7] = g6 * s7;
        mcu[i * 8 + 3] = g7 * s3;
    }
}

// MARK: Quantization ----------------------------------------------------------

void quantize(Mcu &mcu, Quant const &quant) {
    for (usize i = 0; i < 64; ++i) {
        mcu[i] /= quant[i];
    }
}

void dequantize(Mcu &mcu, Quant const &quant) {
    for (usize i = 0; i < 64; ++i) {
        mcu[i] *= quant[i];
    }
}

// MARK: Huffman Tables --------------------------------------------------------

Array<usize, 256> const &Huff::codes() {
    if (not _codes) {
        _codes.emplace();
        usize code = 0;
        for (usize i = 0; i < 16; ++i) {
            for (usize j = offs[i]; j < offs[i + 1]; ++j) {
                (*_codes)[j] = code;
                ++code;
            }
            code <<= 1;
        }
    }
    return *_codes;
}

Res<Byte> Huff::next(BitReader &bs) {
    usize code = 0;
    for (usize i = 0; i < 16; ++i) {
        code = (code << 1) | try$(bs.nextBit());
        for (usize j = offs[i]; j < offs[i + 1]; ++j) {
            if (code == codes()[j]) {
                return Ok(syms[j]);
            }
        }
    }

    logError("jpeg: invalid huffman code {x}", code);
    return Error::invalidData("invalid huffman code");
}

bool Huff::getCode(u8 symbol, usize &code, usize &codeLength) {
    for (usize i = 0; i < 16; ++i) {
        for (usize j = offs[i]; j < offs[i + 1]; ++j) {
            if (symbol == syms[j]) {
                code = codes()[j];
                codeLength = i + 1;
                return true;
            }
        }
    }
    return false;
}
} // namespace Jpeg
