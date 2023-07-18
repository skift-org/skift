#pragma once

#include "../bscan.h"

namespace Ttf {

struct Maxp : public BChunk {
    static constexpr Str SIG = "maxp";

    u16 numGlyphs() const {
        auto s = begin();
        s.skip(4);
        return s.nextU16be();
    }
};

} // namespace Ttf
