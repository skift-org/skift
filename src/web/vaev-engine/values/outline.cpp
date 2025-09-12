module;

#include <karm-gfx/borders.h>
#include <karm-gfx/fill.h>
#include <karm-math/au.h>
#include <karm-math/radii.h>

export module Vaev.Engine:values.outline;

import :values.color;
import :values.lineWidth;

namespace Vaev {

// https://drafts.csswg.org/css-ui/#outline
export struct Outline {
    LineWidth width = Keywords::MEDIUM;
    CalcValue<Length> offset = 0_au;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Union<Keywords::Auto, Color> color = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(outline {} {} {} {})", width, offset, style, color);
    }
};

} // namespace Vaev
