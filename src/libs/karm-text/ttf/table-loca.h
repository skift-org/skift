#pragma once

#include <karm-logger/logger.h>

#include "table-head.h"

namespace Ttf {

struct Loca : Io::BChunk {
    static constexpr Str SIG = "loca";

    usize glyfOffset(isize glyphId, Head const& head) const {
        auto s = begin();
        auto format = head.locaFormat();
        if (format == 0) {
            s.skip(glyphId * 2);
            return s.nextU16be() * 2;
        } else if (format == 1) {
            s.skip(glyphId * 4);
            return s.nextU32be();
        } else {
            logWarn("unsupported loca format {x}", format);
            return 0;
        }
    }
};

} // namespace Ttf
