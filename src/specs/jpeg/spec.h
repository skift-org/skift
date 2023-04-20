#pragma once

#include <karm-base/string.h>
#include <karm-gfx/context.h>
#include <karm-logger/logger.h>

#include "../bscan.h"

namespace Jpeg {

struct YCbCr {
    f64 y;
    f64 cb;
    f64 cr;
};

static constexpr Array<u8, 8 * 8> ZIG_ZAG = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63};

struct Jfif : public BChunk {
    static constexpr Str ID = "JFIF";
};

struct Jfxx : public BChunk {
    static constexpr Str ID = "JFXX";
};

struct Image {
    static constexpr Array<u8, 2> SOI = {0xFF, 0xD8};
    static constexpr Array<u8, 2> SOS = {0xFF, 0xDA};
    static constexpr Array<u8, 2> EOI = {0xFF, 0xD9};

    Bytes _slice;
    Bytes _imageData;

    isize _width;
    isize _height;

    isize width() {
        return 16;
    }

    isize height() {
        return 16;
    }

    static bool isJpeg(Bytes slice) {
        return slice.len() >= 2 and Op::eq(sub(slice, 0, 2), bytes(SOI));
    }

    static Res<Image> load(Bytes slice) {
        Image image{slice};

        return Ok(image);
    }

    Image(Bytes slice) : _slice(slice) {
    }

    BScan begin() {
        return BScan{_slice};
    }

    auto iterSegs() {
        struct Seg {
            Str id;
            Bytes data;
        };

        auto s = begin();
        s.nextI16be(); // SOI

        return Iter{[s]() mutable -> Opt<Seg> {
            Seg seg;

            auto app0 = s.nextBytes(2);

            if (Op::eq(app0, bytes(SOI))) {
                return NONE;
            }

            return seg;
        }};
    }
};

} // namespace Jpeg
