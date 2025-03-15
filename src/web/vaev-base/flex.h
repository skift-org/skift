#pragma once

#include "keywords.h"
#include "sizing.h"

namespace Vaev {

enum struct FlexDirection {
    ROW,
    ROW_REVERSE,
    COLUMN,
    COLUMN_REVERSE,

    _LEN,
};

enum struct FlexWrap {
    NOWRAP,
    WRAP,
    WRAP_REVERSE,

    _LEN
};

using FlexBasis = FlatUnion<Keywords::Auto, Keywords::Content, Size>;

struct FlexItemProps {
    FlexBasis flexBasis = Keywords::AUTO;
    Number flexGrow, flexShrink;

    void repr(Io::Emit& e) const {
        e("({} {} {})", flexBasis, flexGrow, flexShrink);
    }
};

struct FlexProps {
    // FlexContainer
    FlexDirection direction = FlexDirection::ROW;
    FlexWrap wrap = FlexWrap::NOWRAP;

    // FlexItem
    FlexBasis basis = Keywords::AUTO;
    Number grow = 0;
    Number shrink = 1;

    bool isRowOriented() const {
        return direction == FlexDirection::ROW or
               direction == FlexDirection::ROW_REVERSE;
    }

    void repr(Io::Emit& e) const {
        e("(flex");
        e(" direction={}", direction);
        e(" wrap={}", wrap);
        e(" basis={}", basis);
        e(" grow={}", grow);
        e(" shrink={}", shrink);
        e(")");
    }
};

} // namespace Vaev
