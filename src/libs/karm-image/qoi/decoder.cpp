#include "decoder.h"

namespace Qoi {

Karm::Res<Decoder> Decoder::init(Bytes slice) {
    if (slice.len() < 14) {
        return Error::invalidData("image too small");
    }

    auto dec = Decoder(slice);
    if (dec.magic() != MAGIC) {
        return Error::invalidData("invalid magic");
    }

    if (not(dec.channels() == 4 or dec.channels() == 3)) {
        return Error::invalidData("invalid number of channels");
    }

    if (not(dec.colorSpace() == 0 or dec.colorSpace() == 1)) {
        return Error::invalidData("invalid color space");
    }

    return Ok(dec);
}

Res<> Decoder::decode(Gfx::MutPixels dest) {
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

    if (s.nextBytes(8) != END) {
        return Error::invalidData("missing end marker");
    }

    return Ok();
}

} // namespace Qoi
