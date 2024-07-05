#pragma once

#include "table-hhea.h"

namespace Ttf {

struct Hmtx : public Io::BChunk {
    static constexpr Str SIG = "hmtx";
    static constexpr isize LONG_RECORD_SIZE = 4;
    static constexpr isize SHORT_RECORD_SIZE = 2;

    struct Metrics {
        isize advanceWidth;
        isize lsb;
    };

    Metrics metrics(isize glyphId, Hhea const &hhea) const {
        if (glyphId < hhea.numberOfHMetrics()) {
            auto s = begin().skip(glyphId * 4);
            return {s.nextU16be(), s.nextI16be()};
        }

        usize advOffset = (hhea.numberOfHMetrics() - 1) * LONG_RECORD_SIZE;
        usize lsbOffset = hhea.numberOfHMetrics() * LONG_RECORD_SIZE +
                          (glyphId - hhea.numberOfHMetrics()) * SHORT_RECORD_SIZE;

        isize adv = begin().skip(advOffset).nextU16be();
        isize lsb = begin().skip(lsbOffset).nextU16be();

        return {adv, lsb};
    }
};

} // namespace Ttf
