#include "writing.h"

namespace Vaev::Layout {

// https://drafts.csswg.org/css-writing-modes-4/#inline-axis
Axis mainAxis(Box const&) {
    // TODO: Deduce axis

    return Axis::HORIZONTAL;
}

// https://drafts.csswg.org/css-writing-modes-4/#block-axis
Axis crossAxis(Box const&) {
    // TODO: Deduce axis

    return Axis::VERTICAL;
}

} // namespace Vaev::Layout
