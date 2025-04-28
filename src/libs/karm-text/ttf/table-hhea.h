#pragma once

#include <karm-io/bscan.h>

namespace Ttf {

struct Hhea : Io::BChunk {
    static constexpr Str SIG = "hhea";

    isize ascender() const {
        auto s = begin();
        s.skip(4);
        return s.nextI16be();
    }

    isize descender() const {
        auto s = begin();
        s.skip(6);
        return -s.nextI16be();
    }

    isize lineGap() const {
        auto s = begin();
        s.skip(8);
        return s.nextI16be();
    }

    isize advanceWidthMax() const {
        auto s = begin();
        s.skip(10);
        return s.nextU16be();
    }

    isize numberOfHMetrics() const {
        auto s = begin();
        s.skip(34);
        return s.nextU16be();
    }
};

} // namespace Ttf
