#pragma once

// http://www.paulbourke.net/dataformats/tga/
// https://en.wikipedia.org/wiki/Truevision_TGA

#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>
#include <karm-logger/logger.h>

namespace Tga {

enum : usize {
    UNC_CM = 1, // uncompressed color-mapped
    UNC_TC = 2, // uncompressed true-color
    UNC_GS = 3, // uncompressed grayscale

    RLE_CM = 9,  // run-length encoded color-mapped
    RLE_TC = 10, // run-length encoded true-color
    RLE_GS = 11, // run-length encoded grayscale

    ORDER_R2L = (1 << 4), // right-to-left pixel ordering
    ORDER_T2B = (1 << 5), // top-to-bottom pixel ordering

    PACKET_RLE = (1 << 7), // rle/raw field
    PACKET_LEN = 0x7f,     // length mask
};

struct [[gnu::packed]] Header {
    u8le idLen;
    u8le clrmapType;
    u8le imageType;

    u16le cmIndex;
    u16le cmSize;
    u8le cmBpc;

    u16le originX;
    u16le originY;
    u16le width;
    u16le height;
    u8le bpp;
    u8le desc;
};

struct Decoder {
    Header _header;
    Bytes _data;

    static bool sniff(Bytes slice);

    static Res<Decoder> init(Bytes slice);

    Decoder(Header header, Bytes data)
        : _header{header}, _data{data} {}

    isize width() const { return _header.width; }

    isize height() const { return _header.height; }

    static Gfx::Color readColor(Io::BScan &s, usize bpp);

    bool _hasColorMap = false;
    Vec<Gfx::Color> _colorMap = {};

    usize colorMapSize() const;

    Res<> readColorMap(Io::BScan &s);

    Gfx::Color decodePixel(Io::BScan &s);

    void storePixel(Gfx::MutPixels pixels, Math::Vec2i pos, Gfx::Color color);

    void storePixel(Gfx::MutPixels pixels, isize index, Gfx::Color color);

    Res<> decodeUncompress(Io::BScan &s, Gfx::MutPixels pixels);

    Res<> decodeRle(Io::BScan &s, Gfx::MutPixels pixels);

    Res<> decode(Gfx::MutPixels pixels);

    void repr(Io::Emit &e) const;
};

} // namespace Tga
