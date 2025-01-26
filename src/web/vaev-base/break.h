#pragma once

#include <karm-io/emit.h>

#include "numbers.h"

namespace Vaev {

// 3.1. Breaks Between Boxes: the break-before and break-after properties
// https://www.w3.org/TR/css-break-3/#break-between
enum struct BreakBetween {
    AUTO,
    AVOID,
    AVOID_PAGE,
    PAGE,
    LEFT,
    RIGHT,
    RECTO,
    VERSO,
    AVOID_COLUMN,
    COLUMN,
    AVOID_REGION,
    REGION,

    _LEN,
};

// 3.2. Breaks Within Boxes: the break-inside property
// https://www.w3.org/TR/css-break-3/#break-within
enum struct BreakInside {
    AUTO,
    AVOID,
    AVOID_PAGE,
    AVOID_COLUMN,
    AVOID_REGION,

    _LEN,
};

// https://www.w3.org/TR/css-break-3
struct BreakProps {
    BreakBetween before = BreakBetween::AUTO;
    BreakBetween after = BreakBetween::AUTO;
    BreakInside inside = BreakInside::AUTO;

    // Breaks Between Lines: orphans, widows
    // https://www.w3.org/TR/css-break-3/#widows-orphans
    Integer orphans = 2;
    Integer widows = 2;

    void repr(Io::Emit& e) const {
        e("(breaks");
        e(" before={}", before);
        e(" after={}", after);
        e(" inside={}", inside);
        e(" orphans={}", orphans);
        e(" widows={}", widows);
        e(")");
    }
};

} // namespace Vaev
