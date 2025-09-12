#pragma once

import Karm.Core;

#include "base.h"

namespace Karm::Font::Ttf {

struct Post : Io::BChunk {
    static constexpr Str SIG = "post";

    using ItalicAngle = Io::BField<Fixed, 4>;
    using IsFixedPitch = Io::BField<u32be, 12>;

    bool isFixedPitch() const {
        return get<IsFixedPitch>();
    }

    f64 italicAngle() const {
        return get<ItalicAngle>().asF64();
    }
};

} // namespace Karm::Font::Ttf
