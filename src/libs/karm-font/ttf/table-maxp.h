#pragma once

import Karm.Core;

namespace Karm::Font::Ttf {

struct Maxp : Io::BChunk {
    static constexpr Str SIG = "maxp";

    u16 numGlyphs() const {
        auto s = begin();
        s.skip(4);
        return s.nextU16be();
    }
};

} // namespace Karm::Font::Ttf
