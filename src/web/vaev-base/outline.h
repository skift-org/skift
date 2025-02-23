#pragma once

#include <karm-gfx/borders.h>
#include <karm-gfx/fill.h>
#include <karm-math/radii.h>

#include "calc.h"
#include "color.h"
#include "keywords.h"
#include "length.h"

namespace Vaev {

struct Outline {
    CalcValue<Length> width;
    CalcValue<Length> offset;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Union<Keywords::Auto, Color> color = Keywords::Auto{};

    void repr(Io::Emit& e) const {
        e("(outline {} {} {} {})", width, offset, style, color);
    }
};

} // namespace Vaev
