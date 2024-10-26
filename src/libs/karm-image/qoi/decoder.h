#pragma once

#include <karm-base/res.h>
#include <karm-gfx/buffer.h>
#include <karm-gfx/colors.h>
#include <karm-io/bscan.h>

namespace Qoi {

struct Decoder : public Io::BChunk {
    // magic "qoif"
    static constexpr Array<u8, 4> MAGIC = {
        0x71, 0x6F, 0x69, 0x66
    };

    static constexpr Array<u8, 8> END = {
        0, 0, 0, 0, 0, 0, 0, 1
    };

    using Width = Io::BField<i32be, 4>;
    using Height = Io::BField<i32be, 8>;
    using Channels = Io::BField<u8be, 12>;
    using ColorSpace = Io::BField<u8be, 13>;

    Bytes magic() const { return begin().nextBytes(4); }

    isize width() const { return get<Width>(); }

    isize height() const { return get<Height>(); }

    u8 channels() const { return get<Channels>(); }

    u8 colorSpace() const { return get<ColorSpace>(); }

    Decoder(Bytes slice) : BChunk(slice) {}

    static bool sniff(Bytes slice) {
        return slice.len() >= 4 and slice[0] == 0x71 and slice[1] == 0x6F and
               slice[2] == 0x69 and slice[3] == 0x66;
    }

    static Res<Decoder> init(Bytes slice);

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

    Res<> decode(Gfx::MutPixels dest);
};

} // namespace Qoi
