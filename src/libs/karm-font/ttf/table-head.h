#pragma once

import Karm.Core;

namespace Karm::Font::Ttf {

struct Head : Io::BChunk {
    static constexpr Str SIG = "head";

    u16 unitPerEm() const {
        auto s = begin();
        s.skip(18);
        return s.nextU16be();
    }

    u16 locaFormat() const {
        auto s = begin();
        s.skip(50);
        return s.nextU16be();
    }
};

} // namespace Karm::Font::Ttf
