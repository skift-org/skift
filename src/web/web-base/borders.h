#pragma once

#include <karm-gfx/paint.h>

namespace Web {

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
    f64 radiusStart;
    f64 radiusEnd;
    f64 width;
    BorderStyle style;
};

struct Borders {
    Gfx::Paint paint = Gfx::BLACK;
    BorderCollapse collapse;

    Border top;
    Border right;
    Border bottom;
    Border left;

    void all(Border b) {
        top = right = bottom = left = b;
    }
};

} // namespace Web
