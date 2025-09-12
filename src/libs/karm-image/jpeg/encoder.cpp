module;

#include <karm-gfx/buffer.h>

export module Karm.Image:jpeg.encoder;

import Karm.Core;
import :jpeg.base;
import :jpeg.dct;
import :jpeg.tables;

namespace Karm::Image::Jpeg {

namespace {

Res<> _encodeMcu(
    BitWriter& bitWriter,
    Mcu& mcu,
    i16& previousDC,
    Huff& dcTable,
    Huff& acTable
) {
    // encode DC value
    i16 coeff = mcu[0] - previousDC;
    previousDC = mcu[0];

    usize coeffLength = bitLength(Math::abs(coeff));
    if (coeffLength > 11) {
        return Error::invalidData("dc coefficient length exceeds 11 bits");
    }
    if (coeff < 0) {
        coeff += (1 << coeffLength) - 1;
    }

    usize code = 0;
    usize codeLength = 0;
    if (not dcTable.getCode(coeffLength, code, codeLength)) {
        return Error::invalidData("invalid dc huffman code length");
    }
    bitWriter.writeBits(code, codeLength);
    bitWriter.writeBits(coeff, coeffLength);

    // encode AC values
    for (usize i = 1; i < 64; ++i) {
        // find zero run length
        u8 numZeroes = 0;
        while (i < 64 and mcu[ZIGZAG[i]] == 0) {
            numZeroes += 1;
            i += 1;
        }

        if (i == 64) {
            if (not acTable.getCode(0x00, code, codeLength))
                return Error::invalidData("invalid ac huffman code");

            bitWriter.writeBits(code, codeLength);
            return Ok();
        }

        while (numZeroes >= 16) {
            if (not acTable.getCode(0xF0, code, codeLength))
                return Error::invalidData("invalid ac huffman code");

            bitWriter.writeBits(code, codeLength);
            numZeroes -= 16;
        }

        // find coeff length
        coeff = mcu[ZIGZAG[i]];
        coeffLength = bitLength(Math::abs(coeff));
        if (coeffLength > 10) {
            return Error::invalidData("ac coefficient length exceeds 10 bits");
        }
        if (coeff < 0) {
            coeff += (1 << coeffLength) - 1;
        }

        // find symbol in table
        u8 symbol = numZeroes << 4 | coeffLength;
        if (not acTable.getCode(symbol, code, codeLength))
            return Error::invalidData("invalid ac huffman code");

        bitWriter.writeBits(code, codeLength);
        bitWriter.writeBits(coeff, coeffLength);
    }

    return Ok();
}

Res<> _writePixelData(Gfx::Pixels pixels, BitWriter& w) {
    Array<i16, 3> prev = {};

    for (usize y = 0; y < mcuHeight(pixels); y++) {
        for (usize x = 0; x < mcuWidth(pixels); ++x) {
            for (usize i = 0; i < 3; ++i) {
                auto mcu = mcuFetch(pixels, x, y, i);

                fdtc(mcu);

                quantize(mcu, *QUANT100[i]);

                try$(_encodeMcu(
                    w,
                    mcu,
                    prev[i],
                    *HUFF_DC[i],
                    *HUFF_AC[i]
                ));
            }
        }
    }

    return Ok();
}

void _writeQuantizationTable(Io::BEmit& e, u8 tableID, Quant const& qTable) {
    e.writeU8be(0xFF);
    e.writeU8be(DQT);
    e.writeU16be(67);
    e.writeU8be(tableID);
    for (usize i = 0; i < 64; ++i) {
        e.writeU8be(qTable[ZIGZAG[i]]);
    }
}

void _writeStartOfFrame(Io::BEmit& e, Gfx::Pixels pixels) {
    e.writeU8be(0xFF);
    e.writeU8be(SOF0);
    e.writeU16be(17);
    e.writeU8be(8);
    e.writeU16be(pixels.height());
    e.writeU16be(pixels.width());
    e.writeU8be(3);
    for (usize i = 1; i <= 3; ++i) {
        e.writeU8be(i);
        e.writeU8be(0x11);
        e.writeU8be(i == 1 ? 0 : 1);
    }
}

void _writeAPP0(Io::BEmit& e) {
    e.writeU8be(0xFF);
    e.writeU8be(APP0);
    e.writeU16be(16);
    e.writeU8be('J');
    e.writeU8be('F');
    e.writeU8be('I');
    e.writeU8be('F');
    e.writeU8be(0);
    e.writeU8be(1);
    e.writeU8be(2);
    e.writeU8be(0);
    e.writeU16be(100);
    e.writeU16be(100);
    e.writeU8be(0);
    e.writeU8be(0);
}

void _writeHuffmanTable(Io::BEmit& e, u8 acdc, u8 tableID, Huff const& hTable) {
    e.writeU8be(0xFF);
    e.writeU8be(DHT);
    e.writeU16be(19 + hTable.offs[16]);
    e.writeU8be(acdc << 4 | tableID);
    for (usize i = 0; i < 16; ++i) {
        e.writeU8be(hTable.offs[i + 1] - hTable.offs[i]);
    }
    for (usize i = 0; i < 16; ++i) {
        for (usize j = hTable.offs[i]; j < hTable.offs[i + 1]; ++j) {
            e.writeU8be(hTable.syms[j]);
        }
    }
}

void _writeStartOfScan(Io::BEmit& e) {
    e.writeU8be(0xFF);
    e.writeU8be(SOS);
    e.writeU16be(12);
    e.writeU8be(3);
    for (usize i = 1; i <= 3; ++i) {
        e.writeU8be(i);
        e.writeU8be(i == 1 ? 0x00 : 0x11);
    }
    e.writeU8be(0);
    e.writeU8be(63);
    e.writeU8be(0);
}

} // namespace

export Res<> encode(Gfx::Pixels pixels, Io::BEmit& e) {
    // SOI
    e.writeU8be(0xFF);
    e.writeU8be(SOI);

    // APP0
    _writeAPP0(e);

    // DQT
    _writeQuantizationTable(e, 0, QUANT_Y100);
    _writeQuantizationTable(e, 1, QUANT_CbCr100);

    // SOF
    _writeStartOfFrame(e, pixels);

    // DHT
    _writeHuffmanTable(e, 0, 0, HUFF_DC_Y);
    _writeHuffmanTable(e, 0, 1, HUFF_DC_CbCr);
    _writeHuffmanTable(e, 1, 0, HUFF_AC_Y);
    _writeHuffmanTable(e, 1, 1, HUFF_AC_CbCr);

    // SOS
    _writeStartOfScan(e);

    // ECS
    BitWriter w(e);
    try$(_writePixelData(pixels, w));

    // EOI
    e.writeU8be(0xFF);
    e.writeU8be(EOI);

    return Ok();
}

} // namespace Karm::Image::Jpeg
