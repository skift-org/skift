#pragma once

#include "width.h"

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

struct FlexBasis {
    enum struct Type {
        CONTENT,
        WIDTH,
    };

    using enum Type;

    Type type;
    Width width = Width::AUTO;

    constexpr FlexBasis(Type type)
        : type(type) {
    }

    FlexBasis(Width width)
        : type(Type::WIDTH), width(width) {
    }

    void repr(Io::Emit &e) const {
        if (type == Type::CONTENT) {
            e("content");
        } else {
            e("{}", width);
        }
    }
};

struct FlexProps {
    // FlexContainer
    FlexDirection direction = FlexDirection::ROW;
    FlexWrap wrap = FlexWrap::NOWRAP;

    // FlexItem
    FlexBasis basis = Width{Width::AUTO};
    Number grow = 0;
    Number shrink = 1;

    bool isRowOriented() const {
        return direction == FlexDirection::ROW or
               direction == FlexDirection::ROW_REVERSE;
    }

    void repr(Io::Emit &e) const {
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
