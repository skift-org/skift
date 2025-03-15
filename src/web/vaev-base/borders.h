#pragma once

#include <karm-gfx/borders.h>
#include <karm-gfx/fill.h>
#include <karm-math/radii.h>

#include "calc.h"
#include "color.h"
#include "length.h"
#include "line-width.h"
#include "percent.h"

namespace Vaev {

struct Border {
    LineWidth width = Keywords::MEDIUM;
    Gfx::BorderStyle style;
    Color color = Keywords::CURRENT_COLOR;

    void repr(Io::Emit& e) const {
        e("(border {} {} {})", width, style, color);
    }
};

struct BorderProps {
    Border top, start, bottom, end;
    Math::Radii<CalcValue<PercentOr<Length>>> radii = {Length(0_au)};

    void all(Border b) {
        top = start = bottom = end = b;
    }

    void repr(Io::Emit& e) const {
        e("(borders");
        e(" top={}", top);
        e(" start={}", start);
        e(" bottom={}", bottom);
        e(" end={}", end);
        e(" radii={}", radii);
        e(")");
    }
};

} // namespace Vaev
