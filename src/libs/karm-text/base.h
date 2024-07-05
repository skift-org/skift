#pragma once

#include <karm-base/checked.h>
#include <karm-base/distinct.h>

namespace Karm::Text {

struct Glyph : public Distinct<usize, struct _GlyphTag> {
    using Distinct<usize, struct _GlyphTag>::Distinct;

    static Glyph const TOFU;
};

constexpr Glyph Glyph::TOFU{Limits<usize>::MIN};

} // namespace Karm::Text
