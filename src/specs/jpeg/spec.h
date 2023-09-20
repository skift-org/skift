#pragma once

// JPEG Image decoder
// Based on:
//  - https://github.com/klange/toaruos/blob/6a400b399ca35147f89e6bbec017716d1897ea12/lib/jpeg.c
//  - https://en.wikipedia.org/wiki/JPEG_File_Interchange_Format
//  - https://www.w3.org/Graphics/JPEG/itu-t81.pdf
//  - https://github.com/dannye/jed/blob/master/src/decoder.cpp
//  - https://www.youtube.com/watch?v=CPT4FSkFUgs

#include <karm-base/vec.h>
#include <karm-gfx/buffer.h>
#include <karm-gfx/colors.h>
#include <karm-io/bscan.h>
#include <karm-io/emit.h>
#include <karm-logger/logger.h>

namespace Jpeg {

/* --- Constantes ----------------------------------------------------------- */

enum Marker : Byte {
    NIL = 0x00,

    // Start of Frame markers, non-differential, Huffman coding
    SOF0 = 0xC0, // Baseline DCT
    SOF1 = 0xC1, // Extended sequential DCT
    SOF2 = 0xC2, // Progressive DCT
    SOF3 = 0xC3, // Lossless (sequential)

    // Start of Frame markers, differential, Huffman coding
    SOF5 = 0xC5, // Differential sequential DCT
    SOF6 = 0xC6, // Differential progressive DCT
    SOF7 = 0xC7, // Differential lossless (sequential)

    // Start of Frame markers, non-differential, arithmetic coding
    SOF9 = 0xC9,  // Extended sequential DCT
    SOF10 = 0xCA, // Progressive DCT
    SOF11 = 0xCB, // Lossless (sequential)

    // Start of Frame markers, differential, arithmetic coding
    SOF13 = 0xCD, // Differential sequential DCT
    SOF14 = 0xCE, // Differential progressive DCT
    SOF15 = 0xCF, // Differential lossless (sequential)

    // Define Huffman Table(s)
    DHT = 0xC4,

    // JPEG extensions
    JPG = 0xC8,

    // Define Arithmetic Coding Conditioning(s)
    DAC = 0xCC,

    // Restart interval Markers
    RST0 = 0xD0,
    RST1 = 0xD1,
    RST2 = 0xD2,
    RST3 = 0xD3,
    RST4 = 0xD4,
    RST5 = 0xD5,
    RST6 = 0xD6,
    RST7 = 0xD7,

    // Other Markers
    SOI = 0xD8, // Start of Image
    EOI = 0xD9, // End of Image
    SOS = 0xDA, // Start of Scan
    DQT = 0xDB, // Define Quantization Table(s)
    DNL = 0xDC, // Define Number of Lines
    DRI = 0xDD, // Define Restart Interval
    DHP = 0xDE, // Define Hierarchical Progression
    EXP = 0xDF, // Expand Reference Component(s)

    // APPN Markers
    APP0 = 0xE0,
    APP1 = 0xE1,
    APP2 = 0xE2,
    APP3 = 0xE3,
    APP4 = 0xE4,
    APP5 = 0xE5,
    APP6 = 0xE6,
    APP7 = 0xE7,
    APP8 = 0xE8,
    APP9 = 0xE9,
    APP10 = 0xEA,
    APP11 = 0xEB,
    APP12 = 0xEC,
    APP13 = 0xED,
    APP14 = 0xEE,
    APP15 = 0xEF,

    // Misc Markers
    JPG0 = 0xF0,
    JPG1 = 0xF1,
    JPG2 = 0xF2,
    JPG3 = 0xF3,
    JPG4 = 0xF4,
    JPG5 = 0xF5,
    JPG6 = 0xF6,
    JPG7 = 0xF7,
    JPG8 = 0xF8,
    JPG9 = 0xF9,
    JPG10 = 0xFA,
    JPG11 = 0xFB,
    JPG12 = 0xFC,
    JPG13 = 0xFD,
    COM = 0xFE,
    TEM = 0x01,
};

inline constexpr Array<usize, 64> ZIGZAG = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63};

/* --- Bit Stream ----------------------------------------------------------- */

struct BitStream {
    Io::BScan &s;

    u8 _buf = 0x0;
    u8 _len = 0x0;

    ALWAYS_INLINE BitStream(Io::BScan &s) : s(s) {}

    ALWAYS_INLINE void reset() {
        _buf = 0x0;
        _len = 0x0;
    }

    ALWAYS_INLINE Res<> fill() {
        if (_len)
            return Ok();

        _buf = s.nextU8be();
        while (_buf == 0xFF) {
            u8 marker = s.peekU8be();

            while (marker == 0xFF) {
                s.nextU8be();
                marker = s.peekU8be();
            }

            if (marker == 0x00) {
                s.nextU8be();
                break;
            } else if (RST0 <= marker and marker <= RST7) {
                s.nextU8be();
                _buf = s.nextU8be();
            } else {
                logError("jpeg: invalid marker");
                return Error::invalidData("invalid marker");
            }
        }

        _len = 8;
        return Ok();
    }

    ALWAYS_INLINE Res<u8> nextBit() {
        try$(fill());
        u8 bit = _buf >> 7;
        _buf <<= 1;
        _len--;
        return Ok(bit);
    }

    ALWAYS_INLINE Res<usize> nextBits(u8 n) {
        usize res = 0;
        while (n--) {
            res = (res << 1) | try$(nextBit());
        }
        return Ok(res);
    }
};

/* --- Decoder -------------------------------------------------------------- */

struct Image {
    static bool isJpeg(Bytes slice) {
        return slice[0] == 0xFF and slice[1] == SOI;
    }

    static Res<Image> load(Bytes slice) {
        if (!isJpeg(slice)) {
            logError("jpeg: not a JPEG image");
            return Error::invalidData("not a JPEG image");
        }

        Image image{};
        Io::BScan s{slice};

        bool reachedEoi = false;
        bool expectSoi = true;

        while (not s.ended()) {
            u8 first = s.nextU8be();

            if (first != 0xFF) {
                logError("jpeg: invalid marker");
                return Error::invalidData("invalid marker");
            }

            u8 marker = s.nextU8be();

            if (expectSoi) {
                if (marker != SOI) {
                    logError("jpeg: missing SOI marker");
                    return Error::invalidData("missing SOI marker");
                }
                expectSoi = false;
            } else if (APP0 <= marker and marker <= APP15) {
                logWarn("jpeg: skipping APP{}", marker - APP0);
                image.skipMarker(s);
            } else if (marker == DQT) {
                try$(image.defineQuantizationTable(s));
            } else if (marker == SOF0) {
                try$(image.startOfFrame(s));
            } else if (marker == DRI) {
                try$(image.defineRestartInterval(s));
            } else if (marker == DHT) {
                try$(image.defineHuffmanTable(s));
            } else if (marker == SOS) {
                try$(image.startOfScan(s));
                try$(image.decodeHuffman(s));
            } else if (marker == EOI) {
                reachedEoi = true;
            } else if (marker == TEM) {
                logWarn("jpeg: ignoring TEM marker");
            } else if (marker == COM) {
                logDebug("jpeg: skipping comment");
                image.skipMarker(s);
            } else if (marker == 0xff) {
                logDebug("jpeg: skipping padding byte");
                while (marker == 0xff) {
                    marker = s.nextU8be();
                }
            } else {
                logWarn("jpeg: unknown marker: {:02x}", marker);
            }
        }

        if (not reachedEoi) {
            logError("jpeg: missing EOI marker");
            return Error::invalidData("missing EOI marker");
        }

        return Ok(image);
    };

    void skipMarker(Io::BScan &s) {
        u16 len = s.nextU16be();
        s.skip(len - 2);
    }

    /* --- Quantization Tables ---------------------------------------------- */

    using Quant = Array<usize, 64>;
    Array<Opt<Quant>, 4> _quant;
    bool _quirkZeroBased = false;

    Res<> defineQuantizationTable(Io::BScan &x) {
        // logDebug("jpeg: defining quantization table");

        u16 len = x.nextU16be();
        Io::BScan s = x.nextBytes(len - 2);

        while (not s.ended()) {
            Byte infos = s.nextU8be();
            u8 id = infos & 0x0F;

            // logDebug("jpeg: quantization table id: {}", id);

            if (id > 3) {
                logError("jpeg: invalid quantization table id: {}", id);
                return Error::invalidData("invalid quantization table id");
            }

            Quant &quant = _quant[id].emplace();
            bool is16bit = (infos >> 4) == 1;

            for (usize i = 0; i < 64; ++i) {
                // NOTE: Quantization tables are stored in zig-zag order.
                quant[ZIGZAG[i]] = is16bit ? s.nextU16be() : s.nextU8be();
            }
        }

        return Ok();
    }

    /* --- Start of frame --------------------------------------------------- */

    isize _width = 8;
    isize _height = 8;

    isize width() const { return _width; }
    isize height() const { return _height; }

    isize mcuWidth() const { return (_width + 7) / 8; }
    isize mcuHeight() const { return (_height + 7) / 8; }

    struct Component {
        u8 hFactor;
        u8 vFactor;
        u8 quantId;
    };

    Array<Opt<Component>, 4> _components;
    usize _componentCount = 0;

    Res<> startOfFrame(Io::BScan &x) {
        // logDebug("jpeg: start of frame");

        u16 len = x.nextU16be();
        Io::BScan s = x.nextBytes(len - 2);

        u8 precision = s.nextU8be();
        if (precision != 8) {
            logError("jpeg: invalid precision: {}", precision);
            return Error::invalidData("invalid precision");
        }

        _height = s.nextU16be();
        _width = s.nextU16be();

        u8 componentCount = s.nextU8be();
        if (componentCount != 1 and componentCount != 3) {
            logError("jpeg: invalid component count: {}", componentCount);
            return Error::invalidData("invalid component count");
        }

        for (u8 i = 0; i < componentCount; ++i) {
            u8 id = s.nextU8be();

            if (id == 0) {
                logWarn("jpeg: zero-based component id");
                _quirkZeroBased = true;
            }

            if (not _quirkZeroBased) {
                id -= 1;
            }

            if (id > 3) {
                logError("jpeg: invalid component id: {}", id);
                return Error::invalidData("invalid component id");
            }

            if (_components[id]) {
                logError("jpeg: duplicate component id: {}", id);
                return Error::invalidData("duplicate component id");
            }

            u8 factors = s.nextU8be();
            u8 quantId = s.nextU8be();

            _components[id].emplace(Component{
                (u8)(factors >> 4),
                (u8)(factors & 0xF),
                quantId,
            });

            _componentCount = max(_componentCount, (usize)id + 1);
        }

        return Ok();
    }

    /* --- Restart interval ------------------------------------------------- */

    usize _restartInterval = 0;

    Res<> defineRestartInterval(Io::BScan &x) {
        // logDebug("jpeg: defining restart interval");

        u16 len = x.nextU16be();
        Io::BScan s = x.nextBytes(len - 2);

        _restartInterval = s.nextU16be();

        if (not s.ended()) {
            logError("jpeg: unexpected data after DRI marker");
            return Error::invalidData("unexpected data after DRI marker");
        }

        return Ok();
    }

    /* --- Huffman Tables --------------------------------------------------- */

    struct HuffmanTable {
        Array<u8, 17> offs = {};
        Array<u8, 162> syms = {};
        Opt<Array<usize, 256>> _codes = {};

        Array<usize, 256> const &codes() {
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

        Res<Byte> next(BitStream &bs) {
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
    };

    Array<Opt<HuffmanTable>, 4> _dcHuff;
    Array<Opt<HuffmanTable>, 4> _acHuff;

    Res<> defineHuffmanTable(Io::BScan &x) {
        // logDebug("jpeg: defining huffman table");

        u16 len = x.nextU16be();
        Io::BScan s = x.nextBytes(len - 2);

        while (not s.ended()) {
            Byte infos = s.nextU8be();
            u8 id = infos & 0x0F;
            bool isAc = (infos >> 4) == 1;

            if (id > 3) {
                logError("jpeg: invalid huffman table id: {}", id);
                return Error::invalidData("invalid huffman table id");
            }

            HuffmanTable &table = (isAc ? _acHuff : _dcHuff)[id].emplace();

            usize sum = 0;
            for (usize i = 1; i < 17; ++i) {
                sum += s.nextU8be();
                table.offs[i] = sum;
            }

            if (sum > 162) {
                logError("jpeg: invalid huffman table length: {}", sum);
                return Error::invalidData("invalid huffman table length");
            }

            for (usize i = 0; i < sum; ++i) {
                table.syms[i] = s.nextU8be();
            }
        }

        return Ok();
    }

    /* --- Start of scan ---------------------------------------------------- */

    struct ScanComponent {
        u8 dcHuffId;
        u8 acHuffId;
    };

    Array<Opt<ScanComponent>, 4> _scanComponents;
    u8 _ss = 0;
    u8 _se = 0;
    u8 _ah = 0;
    u8 _al = 0;

    Res<> startOfScan(Io::BScan &x) {
        // logDebug("jpeg: start of scan");

        if (_componentCount == 0) {
            logError("jpeg: start of scan before start of frame");
            return Error::invalidData("start of scan before start of frame");
        }

        u16 len = x.nextU16be();
        Io::BScan s = x.nextBytes(len - 2);

        u8 componentCount = s.nextU8be();
        if (componentCount != _componentCount) {
            logError("jpeg: invalid component count: {}", componentCount);
            return Error::invalidData("invalid component count");
        }

        for (u8 i = 0; i < componentCount; ++i) {
            u8 id = s.nextU8be();

            if (not _quirkZeroBased and id == 0) {
                logError("jpeg: component id is zero-based while SOF0 is not");
                return Error::invalidData("component id is zero-based without SOF0");
            }

            if (not _quirkZeroBased) {
                id -= 1;
            }

            if (id > 3) {
                logError("jpeg: invalid component id: {}", id);
                return Error::invalidData("invalid component id");
            }

            if (not _components[id]) {
                logError("jpeg: undefined component id: {}", id);
                return Error::invalidData("undefined component id");
            }

            u8 huffIds = s.nextU8be();
            u8 dcHuffId = huffIds >> 4;
            u8 acHuffId = huffIds & 0xF;

            if (dcHuffId > 3) {
                logError("jpeg: invalid dc huffman table id: {}", dcHuffId);
                return Error::invalidData("invalid dc huffman table id");
            }

            if (acHuffId > 3) {
                logError("jpeg: invalid ac huffman table id: {}", acHuffId);
                return Error::invalidData("invalid ac huffman table id");
            }

            _scanComponents[id].emplace(ScanComponent{dcHuffId, acHuffId});
        }

        _ss = s.nextU8be();
        _se = s.nextU8be();
        u8 ahAl = s.nextU8be();
        _ah = ahAl >> 4;
        _al = ahAl & 0xF;

        if (_ss != 0 or _se != 63) {
            logError("jpeg: unexpected spectral selection");
            return Error::invalidData("unexpected spectral selection");
        }

        if (_ah != 0 or _al != 0) {
            logError("jpeg: unexpected successive approximation");
            return Error::invalidData("unexpected successive approximation");
        }

        if (not s.ended()) {
            logError("jpeg: unexpected data after SOS marker");
            return Error::invalidData("unexpected data after SOS marker");
        }

        return Ok();
    }

    /* --- Huffman Data ----------------------------------------------------- */

    using Mcu = Array<i16, 64>;

    Vec<Mcu> _mcus;

    Res<> decodeHuffman(Io::BScan &s) {
        _mcus.resize(mcuWidth() * mcuHeight() * _componentCount);

        Array<isize, 3> prevDc = {};

        BitStream bs{s};

        for (usize i = 0; i < _mcus.len(); ++i) {
            Mcu &mcu = _mcus[i];
            usize cid = i % _componentCount;

            // logDebug("jpeg: decoding mcu {} of {} (cid: {})", i + 1, _mcus.len(), cid);

            // handle restart interval
            if (_restartInterval > 0 and (i / _componentCount) % _restartInterval == 0) {
                prevDc = {};
                bs.reset();
            }

            if (not _scanComponents[cid]) {
                logError("jpeg: undefined component id: {}", cid);
                return Error::invalidData("undefined component id");
            }

            auto &c = _scanComponents[cid].unwrap();

            // logDebug("jpeg: decoding using huffman table (dc: {}, ac: {})", c.dcHuffId, c.acHuffId);

            if (not _dcHuff[c.dcHuffId]) {
                logError("jpeg: undefined dc huffman table id: {}", c.dcHuffId);
                return Error::invalidData("undefined dc huffman table id");
            }

            auto &dcHuff = _dcHuff[c.dcHuffId].unwrap();

            if (not _acHuff[c.acHuffId]) {
                logError("jpeg: undefined ac huffman table id: {}", c.acHuffId);
                return Error::invalidData("undefined ac huffman table id");
            }

            auto &acHuff = _acHuff[c.acHuffId].unwrap();

            Byte len = try$(dcHuff.next(bs));

            if (len > 11) {
                logError("jpeg: invalid dc huffman code length: {}", len);
                return Error::invalidData("invalid dc huffman code length");
            }

            isize coeff = try$(bs.nextBits(len));

            if (len != 0 and coeff < (1 << (len - 1))) {
                coeff -= (1 << len) - 1;
            }

            mcu[0] = prevDc[cid] + coeff;
            prevDc[cid] = mcu[0];

            usize k = 1;
            while (k < 64) {
                Byte sym = try$(acHuff.next(bs));

                if (sym == 0) {
                    break;
                }

                Byte numZeroes = sym >> 4;

                if (sym == 0xF0) {
                    numZeroes = 16;
                }

                if (k + numZeroes >= 64) {
                    logError("jpeg: zero run length exceeds block size: {}", k + numZeroes);
                    return Error::invalidData("zero run length exceeds block size");
                }

                k += numZeroes;

                Byte len = sym & 0xF;

                if (len > 10) {
                    logError("jpeg: invalid ac huffman code length: {}", len);
                    return Error::invalidData("invalid ac huffman code length");
                }

                if (len) {
                    coeff = try$(bs.nextBits(len));

                    if (coeff < (1 << (len - 1))) {
                        coeff -= (1 << len) - 1;
                    }

                    mcu[ZIGZAG[k++]] = coeff;
                }
            }
        }

        return Ok();
    }

    /* --- Decoding --------------------------------------------------------- */

    void idct(Array<short, 64> &mcu) {
        f32 m0 = 2.0 * Math::cos(1.0 / 16.0 * 2.0 * Math::PI);
        f32 m1 = 2.0 * Math::cos(2.0 / 16.0 * 2.0 * Math::PI);
        f32 m3 = 2.0 * Math::cos(2.0 / 16.0 * 2.0 * Math::PI);
        f32 m5 = 2.0 * Math::cos(3.0 / 16.0 * 2.0 * Math::PI);
        f32 m2 = m0 - m5;
        f32 m4 = m0 + m5;

        f32 s0 = Math::cos(0.0 / 16.0 * Math::PI) / Math::sqrt(8.0);
        f32 s1 = Math::cos(1.0 / 16.0 * Math::PI) / 2.0;
        f32 s2 = Math::cos(2.0 / 16.0 * Math::PI) / 2.0;
        f32 s3 = Math::cos(3.0 / 16.0 * Math::PI) / 2.0;
        f32 s4 = Math::cos(4.0 / 16.0 * Math::PI) / 2.0;
        f32 s5 = Math::cos(5.0 / 16.0 * Math::PI) / 2.0;
        f32 s6 = Math::cos(6.0 / 16.0 * Math::PI) / 2.0;
        f32 s7 = Math::cos(7.0 / 16.0 * Math::PI) / 2.0;

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

    Res<> decode(Gfx::MutPixels pixels) {
        for (usize i = 0; i < _mcus.len(); i += _componentCount) {
            usize x = (i / _componentCount) % mcuWidth();
            usize y = (i / _componentCount) / mcuWidth();

            for (usize j = 0; j < _componentCount; j++) {
                auto &mcu = _mcus[i + j];

                if (not _quant[_components[j]->quantId]) {
                    logError("jpeg: undefined quantization table id: {}", _components[j]->quantId);
                    return Error::invalidData("undefined quantization table id");
                }

                auto &quant = _quant[_components[j]->quantId].unwrap();
                for (usize k = 0; k < 64; k++) {
                    mcu[k] *= quant[k];
                }

                idct(mcu);
            }

            auto &yMcu = _mcus[i + 0];
            auto &cbMcu = _mcus[i + 1];
            auto &crMcu = _mcus[i + 2];

            for (usize k = 0; k < 64; ++k) {
                isize px = x * 8 + k % 8;
                isize py = y * 8 + k / 8;

                Gfx::YCbCr ycbcr{(float)yMcu[k], (float)cbMcu[k], (float)crMcu[k]};
                pixels.storeUnsafe({px, py}, Gfx::yCbCrToRgb(ycbcr));
            }
        }

        return Ok();
    }

    /* --- Dumping ---------------------------------------------------------- */

    void dump(Io::Emit &e) {
        e("JPEG image");
        e.indentNewline();
        e.ln("width: {}", width());
        e.ln("height: {}", height());

        e("quantization tables:");
        e.indentNewline();

        e("zero-based (quirk): {}\n", _quirkZeroBased);
        for (usize i = 0; i < _quant.len(); ++i) {
            if (_quant[i]) {
                e.ln("table {}:", i);
                e.indent();
                for (usize j = 0; j < 64; ++j) {
                    e("{02} ", (*_quant[i])[j]);
                    if (j % 8 == 7)
                        e.newline();
                }
                e.newline();
                e.deindent();
            } else {
                e("table {}: none\n", i);
            }
        }
        e.deindent();

        e("components:");
        e.indentNewline();
        for (usize i = 0; i < _components.len(); ++i) {
            if (_components[i]) {
                e("component {}:", i);
                e.indentNewline();
                e.ln("hFactor: {}", (*_components[i]).hFactor);
                e.ln("vFactor: {}", (*_components[i]).vFactor);
                e.ln("quantId: {}", (*_components[i]).quantId);
                e.deindent();
            } else {
                e("component {}: none\n", i);
            }
        }
        e.deindent();

        e("restart interval: {}\n", _restartInterval);

        e("huffman tables:");
        e.indentNewline();

        for (usize i = 0; i < 4; i++) {
            if (_dcHuff[i]) {
                auto table = (*_dcHuff[i]);
                e("DC table {}:\n", i);
                e.indent();
                for (usize j = 0; j < 16; ++j) {
                    e("{02x}:", table.offs[j]);
                    for (usize k = table.offs[j]; k < table.offs[j + 1]; ++k) {
                        e(" {02x}", table.syms[k]);
                    }
                    e.newline();
                }
                e.deindent();
            } else {
                e("DC table {}: none\n", i);
            }
        }

        for (usize i = 0; i < 4; i++) {
            if (_acHuff[i]) {
                auto table = (*_acHuff[i]);
                e.ln("AC table {}:", i);
                e.indent();
                for (usize j = 0; j < 16; ++j) {
                    e("{02x}:", table.offs[j]);
                    for (usize k = table.offs[j]; k < table.offs[j + 1]; ++k) {
                        e(" {02x}", table.syms[k]);
                    }
                    e.newline();
                }
                e.deindent();
            } else {
                e.ln("AC table {}: none", i);
            }
        }

        e.deindent();

        e("scan components:");
        e.indentNewline();
        for (usize i = 0; i < _scanComponents.len(); ++i) {
            if (_scanComponents[i]) {
                e.ln("component {}:", i);
                e.indent();
                e.ln("dcHuffId: {}", (*_scanComponents[i]).dcHuffId);
                e.ln("acHuffId: {}", (*_scanComponents[i]).acHuffId);
                e.deindent();
            } else {
                e.ln("component {}: none", i);
            }
        }

        e.ln("ss: {}", _ss);
        e.ln("se: {}", _se);
        e.ln("ah: {}", _ah);
        e.ln("al: {}", _al);

        e.deindent();
    }
};

} // namespace Jpeg
