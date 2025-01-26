#pragma once

#include "length.h"
#include "percent.h"
#include "width.h"

namespace Vaev {

enum struct TableLayout {
    AUTO,
    FIXED,

    _LEN
};

enum struct CaptionSide {
    TOP,
    BOTTOM,

    _LEN
};

enum struct BorderCollapse {
    SEPARATE,
    COLLAPSE,
};

struct BorderSpacing {
    Length horizontal, vertical;

    void repr(Io::Emit& e) const {
        e("({}, {})", horizontal, vertical);
    }
};

struct TableProps {
    TableLayout tableLayout = TableLayout::AUTO;
    CaptionSide captionSide = CaptionSide::TOP;
    BorderSpacing spacing = {0_px, 0_px};
    BorderCollapse collapse = BorderCollapse::SEPARATE;

    void repr(Io::Emit& e) const {
        e("(table");
        e(" tableLayout={}", tableLayout);
        e(" captionSide={}", captionSide);
        e(" spacing={}", spacing);
        e(" collapse={}", collapse);
        e(")");
    }
};

} // namespace Vaev
