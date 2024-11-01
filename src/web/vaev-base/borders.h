#pragma once

#include <karm-gfx/borders.h>
#include <karm-gfx/fill.h>
#include <karm-math/radii.h>

#include "color.h"
#include "length.h"
#include "percent.h"

namespace Vaev {

enum struct BorderEdge {
    ALL,

    TOP,
    END,
    BOTTOM,
    START,
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
    Length width;
    Gfx::BorderStyle style;
    Color color = Color::CURRENT;

    void repr(Io::Emit &e) const {
        e("{}-{}-{}", width, style, color);
    }
};

struct BorderProps {
    static constexpr Length THIN = Px{1};
    static constexpr Length MEDIUM = Px{3};
    static constexpr Length THICK = Px{5};

    Border top, start, bottom, end;
    Math::Radii<PercentOr<Length>> radii;

    void all(Border b) {
        top = start = bottom = end = b;
    }

    void set(BorderEdge edge, Border b) {
        switch (edge) {
        case BorderEdge::ALL:
            all(b);
            break;
        case BorderEdge::TOP:
            top = b;
            break;
        case BorderEdge::START:
            start = b;
            break;
        case BorderEdge::BOTTOM:
            bottom = b;
            break;
        case BorderEdge::END:
            end = b;
            break;
        }
    }
};

} // namespace Vaev
