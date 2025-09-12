#pragma once

import Karm.Core;

namespace Karm::Font::Ttf {

// https://learn.microsoft.com/en-us/typography/opentype/spec/hhea
struct Hhea : Io::BChunk {
    static constexpr Str SIG = "hhea";

    using Ascender = Io::BField<i16be, 4>;
    using Descender = Io::BField<i16be, 6>;
    using LineGap = Io::BField<i16be, 8>;
    using AdvanceWidthMax = Io::BField<u16be, 10>;
    using NumberOfHMetrics = Io::BField<u16be, 34>;

    isize ascender() const {
        return get<Ascender>();
    }

    isize descender() const {
        return -get<Descender>();
    }

    isize lineGap() const {
        return get<LineGap>();
    }

    isize advanceWidthMax() const {
        return get<AdvanceWidthMax>();
    }

    isize numberOfHMetrics() const {
        return get<NumberOfHMetrics>();
    }
};

} // namespace Karm::Font::Ttf
