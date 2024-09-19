#pragma once

// http://www.paulbourke.net/dataformats/tga/
// https://en.wikipedia.org/wiki/Truevision_TGA

#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>
#include <karm-logger/logger.h>

#include "base.h"

namespace Tga {

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
