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

    void repr(Io::Emit &e) const {
        e("({}, {})", horizontal, vertical);
    }
};

struct TableProps {
    TableLayout tableLayout = TableLayout::AUTO;
    CaptionSide captionSide = CaptionSide::TOP;
    BorderSpacing spacing = {Px{0}, Px{0}};
    BorderCollapse collapse = BorderCollapse::SEPARATE;
};

} // namespace Vaev
