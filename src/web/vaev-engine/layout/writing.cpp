module;

export module Vaev.Engine:layout.writing;

import :values;

import :layout.base;

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
