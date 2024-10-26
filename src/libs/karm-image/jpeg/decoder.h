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

// MARK: Constantes ------------------------------------------------------------

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
    53, 60, 61, 54, 47, 55, 62, 63
};

// MARK: Bit Stream ------------------------------------------------------------

struct BitStream {
    Io::BScan &s;

    u8 _buf = 0x0;
    u8 _len = 0x0;

    always_inline BitStream(Io::BScan &s) : s(s) {}

    always_inline void reset() {
        _buf = 0x0;
        _len = 0x0;
    }

    always_inline Res<> fill() {
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

    always_inline Res<u8> nextBit() {
        try$(fill());
        u8 bit = _buf >> 7;
        _buf <<= 1;
        _len--;
        return Ok(bit);
    }

    always_inline Res<usize> nextBits(u8 n) {
        usize res = 0;
        while (n--) {
            res = (res << 1) | try$(nextBit());
        }
        return Ok(res);
    }
};

// MARK: Decoder ---------------------------------------------------------------

struct Decoder {
    static bool sniff(Bytes slice);

    static Res<Decoder> init(Bytes slice);

    void skipMarker(Io::BScan &s);

    // MARK: Quantization Tables -----------------------------------------------

    using Quant = Array<usize, 64>;
    Array<Opt<Quant>, 4> _quant;
    bool _quirkZeroBased = false;

    Res<> defineQuantizationTable(Io::BScan &x);

    // MARK: Start of frame ----------------------------------------------------

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

    Res<> startOfFrame(Io::BScan &x);

    // MARK: Restart interval --------------------------------------------------

    usize _restartInterval = 0;

    Res<> defineRestartInterval(Io::BScan &x);

    // MARK: Huffman Tables ----------------------------------------------------

    struct HuffmanTable {
        Array<u8, 17> offs = {};
        Array<u8, 162> syms = {};
        Opt<Array<usize, 256>> _codes = {};

        Array<usize, 256> const &codes();

        Res<Byte> next(BitStream &bs);
    };

    Array<Opt<HuffmanTable>, 4> _dcHuff;
    Array<Opt<HuffmanTable>, 4> _acHuff;

    Res<> defineHuffmanTable(Io::BScan &x);

    // MARK: Start of scan -----------------------------------------------------

    struct ScanComponent {
        u8 dcHuffId;
        u8 acHuffId;
    };

    Array<Opt<ScanComponent>, 4> _scanComponents;
    u8 _ss = 0;
    u8 _se = 0;
    u8 _ah = 0;
    u8 _al = 0;

    Res<> startOfScan(Io::BScan &x);

    // MARK: Huffman Data ------------------------------------------------------

    using Mcu = Array<i16, 64>;

    Vec<Mcu> _mcus;

    Res<> decodeHuffman(Io::BScan &s);

    // MARK: Decoding ----------------------------------------------------------

    void idct(Array<short, 64> &mcu);

    Res<> decode(Gfx::MutPixels pixels);

    // MARK: Dumping -----------------------------------------------------------

    void repr(Io::Emit &e);
};

} // namespace Jpeg
