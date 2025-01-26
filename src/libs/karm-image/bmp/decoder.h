#pragma once

// BMP image decoder
// References:
//  - https://en.wikipedia.org/wiki/BMP_file_format
//  - https://docs.microsoft.com/en-us/windows/win32/gdi/bitmap-storage
//  - http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
//  - http://www.martinreddy.net/gfx/2d/BMP.txt

#include <karm-base/vec.h>
#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>
#include <karm-logger/logger.h>

namespace Bmp {

struct Decoder {

    // MARK: Loading -----------------------------------------------------------

    static bool sniff(Bytes slice) {
        return slice.len() >= 2 and
               slice[0] == 0x42 and
               slice[1] == 0x4D;
    }

    static Res<Decoder> init(Bytes slice);

    // MARK: Header ------------------------------------------------------------

    usize _dataOffset;

    Res<> _readHeader(Io::BScan& s);

    isize _width;
    isize _height;
    isize _bpp;

    isize width() const {
        return Math::abs(_width);
    }

    isize height() const {
        return Math::abs(_height);
    }

    enum Compression {
        RGB = 0,
        RLE8 = 1,
        RLE4 = 2,
    } _compression;

    usize _numsColors;

    Res<> _readInfoHeader(Io::BScan& s);

    // MARK: Palette -----------------------------------------------------------

    Vec<Gfx::Color> _palette;

    Res<> _readPalette(Io::BScan& s);

    // MARK: Pixels ------------------------------------------------------------

    Bytes _pixels;

    Res<> _readPixels(Io::BScan& s);

    // MARK: Decoding ----------------------------------------------------------

    Res<> decode(Gfx::MutPixels pixels);

    // MARK: Dumping -----------------------------------------------------------

    void repr(Io::Emit& e);
};

} // namespace Bmp
