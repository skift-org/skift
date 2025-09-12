module;

#include <karm-gfx/buffer.h>

export module Karm.Image:bmp.encoder;

import Karm.Core;

namespace Karm::Image::Bmp {

namespace {

void _writePixelData(Gfx::Pixels pixels, Io::BEmit& e) {
    for (isize y = pixels.height() - 1; y >= 0; --y) {
        for (isize x = 0; x < pixels.width(); ++x) {
            auto color = pixels.load({x, y});
            e.writeU8le(color.blue);
            e.writeU8le(color.green);
            e.writeU8le(color.red);
            e.writeU8le(color.alpha);
        }
    }
}

} // namespace

export Res<> encode(Gfx::Pixels pixels, Io::BEmit& e) {
    if (pixels.width() < 0 or pixels.height() < 0)
        return Error::invalidData("negative dimensions");

    if (pixels.width() > Limits<i32>::MAX or pixels.height() > Limits<i32>::MAX)
        return Error::invalidData("dimensions too large");

    usize headerSize = 14;
    usize infoSize = 40;
    usize pixelOffset = headerSize + infoSize;

    Io::BufferWriter pixelData{(usize)pixels.width() * (usize)pixels.height() * 4};
    Io::BEmit p{pixelData};
    _writePixelData(pixels, p);

    usize fileSize = pixelOffset + pixelData.bytes().len();

    e.writeStr("BM"s);
    e.writeU32le(fileSize);    // file size
    e.writeU32le(0);           // reserved
    e.writeU32le(pixelOffset); // pixel offset

    e.writeU32le(infoSize);        // info size
    e.writeI32le(pixels.width());  // width
    e.writeI32le(pixels.height()); // height
    e.writeU16le(1);               // planes
    e.writeU16le(32);              // bpp
    e.writeU32le(0);               // compression
    e.writeU32le(0);               // image size
    e.writeI32le(2835);            // x pixels per meter
    e.writeI32le(2835);            // y pixels per meter
    e.writeU32le(0);               // colors used
    e.writeU32le(0);               // important colors

    e.writeBytes(pixelData.bytes());

    return Ok();
}

} // namespace Karm::Image::Bmp
