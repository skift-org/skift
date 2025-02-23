module;

#include <vaev-base/writing.h>

export module Vaev.Layout:writing;

import :base;

namespace Vaev::Layout {

// https://drafts.csswg.org/css-writing-modes-4/#inline-axis
export Axis mainAxis(Box const&) {
    // TODO: Deduce axis

    return Axis::HORIZONTAL;
}

// https://drafts.csswg.org/css-writing-modes-4/#block-axis
export Axis crossAxis(Box const&) {
    // TODO: Deduce axis

    return Axis::VERTICAL;
}

} // namespace Vaev::Layout
