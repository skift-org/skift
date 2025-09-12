#pragma once

import Karm.Core;

#include "base.h"

namespace Karm::Font::Ttf {

struct Os2 : Io::BChunk {
    static constexpr Str SIG = "OS/2";

    using WeightClass = Io::BField<u16be, 4>;
    using WidthClass = Io::BField<u16be, 6>;

    u16 weightClass() const {
        return get<WeightClass>();
    }

    u16 widthClass() const {
        return get<WidthClass>();
    }
};

} // namespace Karm::Font::Ttf
