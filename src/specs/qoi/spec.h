#pragma once

#include <karm-base/res.h>
#include <karm-gfx/buffer.h>
#include <karm-gfx/colors.h>
#include <karm-io/bscan.h>

namespace Qoi {

struct Image : public Io::BChunk {
    // magic "qoif"
    static constexpr Array<u8, 4> MAGIC = {
        0x71, 0x6F, 0x69, 0x66};

    static constexpr Array<u8, 8> END = {
        0, 0, 0, 0, 0, 0, 0, 1};

    using Width = Io::BField<i32be, 4>;
    using Height = Io::BField<i32be, 8>;
    using Channels = Io::BField<u8be, 12>;
    using ColorSpace = Io::BField<u8be, 13>;

    Bytes magic() const { return begin().nextBytes(4); }

    isize width() const { return get<Width>(); }

    isize height() const { return get<Height>(); }

    u8 channels() const { return get<Channels>(); }

    u8 colorSpace() const { return get<ColorSpace>(); }

    Image(Bytes slice) : BChunk(slice) {}

    static bool isQoi(Bytes slice) {
        return slice.len() >= 4 and slice[0] == 0x71 and slice[1] == 0x6F and
               slice[2] == 0x69 and slice[3] == 0x66;
    }

    static Res<Image> load(Bytes slice) {
        if (slice.len() < 14) {
            return Error::invalidData("image too small");
        }

        auto image = Image(slice);
        if (image.magic() != MAGIC) {
            return Error::invalidData("invalid magic");
        }

        if (not(image.channels() == 4 or image.channels() == 3)) {
            return Error::invalidData("invalid number of channels");
        }

        if (not(image.colorSpace() == 0 or image.colorSpace() == 1)) {
            return Error::invalidData("invalid color space");
        }

        return Ok(image);
    }

    enum Chunk : u8 {
        RGB = 0b11111110,
        RGBA = 0b11111111,
        INDEX = 0b00000000,
        DIFF = 0b01000000,
        LUMA = 0b10000000,
        RUN = 0b11000000,

        MASK = 0b11000000,
    };

    usize hash(Gfx::Color c) {
        return c.red * 3 + c.green * 5 + c.blue * 7 + c.alpha * 11;
    }

    [[gnu::flatten]] Res<> decode(Gfx::MutPixels dest) {
        usize run = 0;
        Array<Gfx::Color, 64> index{};
        Gfx::Color pixel = Gfx::BLACK;

        auto s = begin().skip(14);

        for (isize y = 0; y < height(); y++) {
            for (isize x = 0; x < width(); x++) {
                if (s.ended()) {
                    return Error::invalidData("unexpected end of file");
                }

                if (run > 0) {
                    run--;
                    dest.store({x, y}, pixel);
                    continue;
                }

                auto b1 = s.nextU8be();
                if (b1 == Chunk::RGB) {
                    pixel.red = s.nextU8be();
                    pixel.green = s.nextU8be();
                    pixel.blue = s.nextU8be();
                } else if (b1 == Chunk::RGBA) {
                    pixel.red = s.nextU8be();
                    pixel.green = s.nextU8be();
                    pixel.blue = s.nextU8be();
                    pixel.alpha = s.nextU8be();
                } else if ((b1 & Chunk::MASK) == Chunk::INDEX) {
                    pixel = index[b1];
                } else if ((b1 & Chunk::MASK) == Chunk::DIFF) {
                    pixel.red += ((b1 >> 4) & 0x03) - 2;
                    pixel.green += ((b1 >> 2) & 0x03) - 2;
                    pixel.blue += (b1 & 0x03) - 2;
                } else if ((b1 & Chunk::MASK) == Chunk::LUMA) {
                    auto b2 = s.nextU8be();
                    auto vg = (b1 & 0x3f) - 32;
                    pixel.red += vg - 8 + ((b2 >> 4) & 0x0f);
                    pixel.green += vg;
                    pixel.blue += vg - 8 + (b2 & 0x0f);
                } else if ((b1 & Chunk::MASK) == Chunk::RUN) {
                    run = b1 & (~Chunk::MASK);
                } else {
                    return Error::invalidData("invalid chunk");
                }

                index[hash(pixel) % index.len()] = pixel;
                dest.store({x, y}, pixel);
            }
        }

        if (s.nextBytes(8) != bytes(END)) {
            return Error::invalidData("missing end marker");
        }

        return Ok();
    }
};

} // namespace Qoi
