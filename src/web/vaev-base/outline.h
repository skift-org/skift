#pragma once

#include <karm-gfx/borders.h>
#include <karm-gfx/fill.h>
#include <karm-math/radii.h>

#include "color.h"
#include "line-width.h"

namespace Vaev {

// https://drafts.csswg.org/css-ui/#outline
struct Outline {
    LineWidth width = Keywords::MEDIUM;
    CalcValue<Length> offset = 0_au;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Union<Keywords::Auto, Color> color = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(outline {} {} {} {})", width, offset, style, color);
    }
};

} // namespace Vaev
