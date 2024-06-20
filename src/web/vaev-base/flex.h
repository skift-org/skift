#pragma once

#include "length.h"
#include "percent.h"

namespace Vaev {

enum struct FlexDirection {
    ROW,
    ROW_REVERSE,
    COLUMN,
    COLUMN_REVERSE,
};

enum struct FlexWrap {
    NOWRAP,
    WRAP,
    WRAP_REVERSE,
};

struct Flex {
    FlexDirection direction = FlexDirection::ROW;
    FlexWrap wrap = FlexWrap::NOWRAP;
    f64 flexGrow = 0;
    f64 flexShrink = 1;
};

} // namespace Vaev
