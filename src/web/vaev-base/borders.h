#pragma once

#include <karm-gfx/paint.h>
#include <karm-math/radius.h>

#include "length.h"
#include "percent.h"

namespace Vaev {

enum struct BorderEdge {
    ALL,

    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
};

enum struct BorderCollapse {
    SEPARATE,
    COLLAPSE,
};

enum struct BorderStyle {
    NONE,

    HIDDEN,
    DOTTED,
    DASHED,
    SOLID,
    DOUBLE,
    GROOVE,
    RIDGE,
    INSET,
    OUTSET,
};

struct Border {
    f64 width;
    BorderStyle style;
};

struct Borders {
    Gfx::Paint paint = Gfx::BLACK;
    BorderCollapse collapse;

    Border top, right, bottom, left;
    Math::Radius<PercentOr<Length>> radii;

    constexpr void all(Border b) {
        top = right = bottom = left = b;
    }

    constexpr void set(BorderEdge edge, Border b) {
        switch (edge) {
        case BorderEdge::ALL:
            all(b);
            break;
        case BorderEdge::TOP:
            top = b;
            break;
        case BorderEdge::RIGHT:
            right = b;
            break;
        case BorderEdge::BOTTOM:
            bottom = b;
            break;
        case BorderEdge::LEFT:
            left = b;
            break;
        }
    }
};

} // namespace Vaev
