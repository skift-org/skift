#pragma once

// https://learn.microsoft.com/en-us/typography/opentype/spec/cpal

#include <karm-io/bscan.h>

namespace Ttf {

struct ColorRecord {
    u8 red, green, blue, alpha;

    static ColorRecord read(Io::BScan &scan) {
        return {
            .red = scan.nextU8be(),
            .green = scan.nextU8be(),
            .blue = scan.nextU8be(),
            .alpha = scan.nextU8be(),
        };
    }
};

struct Cpal : public Io::BChunk {
    static constexpr Str SIG = "CPAL";

    using Version = Io::BField<u16be, 0>;
};

} // namespace Ttf
