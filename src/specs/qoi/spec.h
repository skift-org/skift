#pragma once

#include <karm-base/result.h>
#include <karm-gfx/buffer.h>

#include "../bscan.h"

namespace Qoi {

struct Image {
    // magic "qoif"
    static constexpr Array<uint8_t, 4> MAGIC = {
        0x71, 0x6F, 0x69, 0x66};

    static constexpr Array<uint8_t, 8> END = {
        0, 0, 0, 0, 0, 0, 0, 1};

    Bytes _slice;

    BScan begin() const { return _slice; }

    Bytes magic() const { return begin().nextBytes(4); }

    int width() const { return begin().skip(4).nextBeInt32(); }

    int height() const { return begin().skip(8).nextBeInt32(); }

    int channels() const { return begin().skip(12).nextBeUint8(); }

    int colorSpace() const { return begin().skip(13).nextBeUint8(); }

    Image(Bytes slice) : _slice(slice) {}

    static bool isQoi(Bytes const &slice) {
        return slice.len() >= 4 and slice[0] == 0x71 and slice[1] == 0x6F and
               slice[2] == 0x69 and slice[3] == 0x66;
    }

    static Result<Image> load(Bytes slice) {
        if (slice.len() < 14) {
            return Error("image too small");
        }

        auto image = Image(slice);
        if (Op::ne(image.magic(), bytes(MAGIC))) {
            return Error("invalid magic");
        }

        if (!(image.channels() == 4 or image.channels() == 3)) {
            return Error("invalid number of channels");
        }

        if (!(image.colorSpace() == 0 or image.colorSpace() == 1)) {
            return Error("invalid color space");
        }

        return image;
    }

    enum Chunk : uint8_t {
        RGB = 0b11111110,
        RGBA = 0b11111111,
        INDEX = 0b00000000,
        DIFF = 0b01000000,
        LUMA = 0b10000000,
        RUN = 0b11000000,

        MASK = 0b11000000,
    };

    size_t hash(Gfx::Color c) {
        return c.red * 3 + c.green * 5 + c.blue * 7 + c.alpha * 11;
    }

    [[gnu::flatten]] Error decode(Gfx::Surface dest) {
        size_t run = 0;
        Array<Gfx::Color, 64> index{};
        Gfx::Color pixel = Gfx::BLACK;

        auto s = begin().skip(14);

        for (int y = 0; y < height(); y++) {
            for (int x = 0; x < width(); x++) {
                if (s.ended()) {
                    return Error("unexpected end of file");
                }

                if (run > 0) {
                    run--;
                    dest.store({x, y}, pixel);
                    continue;
                }

                auto b1 = s.nextBeUint8();
                if (b1 == Chunk::RGB) {
                    pixel.red = s.nextBeUint8();
                    pixel.green = s.nextBeUint8();
                    pixel.blue = s.nextBeUint8();
                } else if (b1 == Chunk::RGBA) {
                    pixel.red = s.nextBeUint8();
                    pixel.green = s.nextBeUint8();
                    pixel.blue = s.nextBeUint8();
                    pixel.alpha = s.nextBeUint8();
                } else if ((b1 & Chunk::MASK) == Chunk::INDEX) {
                    pixel = index[b1];
                } else if ((b1 & Chunk::MASK) == Chunk::DIFF) {
                    pixel.red += ((b1 >> 4) & 0x03) - 2;
                    pixel.green += ((b1 >> 2) & 0x03) - 2;
                    pixel.blue += (b1 & 0x03) - 2;
                } else if ((b1 & Chunk::MASK) == Chunk::LUMA) {
                    auto b2 = s.nextBeUint8();
                    auto vg = (b1 & 0x3f) - 32;
                    pixel.red += vg - 8 + ((b2 >> 4) & 0x0f);
                    pixel.green += vg;
                    pixel.blue += vg - 8 + (b2 & 0x0f);
                } else if ((b1 & Chunk::MASK) == Chunk::RUN) {
                    run = b1 & (~Chunk::MASK);
                } else {
                    return "invalid chunk";
                }

                index[hash(pixel) % index.len()] = pixel;
                dest.store({x, y}, pixel);
            }
        }

        if (Op::ne(s.nextBytes(8), bytes(END))) {
            return "missing end marker";
        }

        return OK;
    }
};

} // namespace Qoi
