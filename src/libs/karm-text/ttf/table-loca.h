#pragma once

#include "table-head.h"

namespace Ttf {

struct Loca : public Io::BChunk {
    static constexpr Str SIG = "loca";

    usize glyfOffset(isize glyphId, Head const &head) const {
        auto s = begin();
        if (head.locaFormat() == 0) {
            s.skip(glyphId * 2);
            return s.nextU16be();
        } else {
            s.skip(glyphId * 4);
            return s.nextU32be();
        }
    }
};

} // namespace Ttf
