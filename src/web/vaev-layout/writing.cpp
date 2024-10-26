#include "writing.h"

namespace Vaev::Layout {

// https://drafts.csswg.org/css-writing-modes-4/#inline-axis
Axis mainAxis(Frag &) {
    // TODO: Deduce axis

    return Axis::HORIZONTAL;
}

// https://drafts.csswg.org/css-writing-modes-4/#block-axis
Axis crossAxis(Frag &) {
    // TODO: Deduce axis

    return Axis::VERTICAL;
}

} // namespace Vaev::Layout
