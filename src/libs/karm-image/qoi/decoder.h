#pragma once

#include <karm-base/res.h>
#include <karm-gfx/buffer.h>
#include <karm-gfx/colors.h>
#include <karm-io/bscan.h>

#include "base.h"

namespace Qoi {

struct Decoder : Io::BChunk {
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

    Res<> decode(Gfx::MutPixels dest);
};

} // namespace Qoi
