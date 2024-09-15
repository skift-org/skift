#pragma once

#include "length.h"
#include "percent.h"
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

struct Flex {
    FlexDirection direction = FlexDirection::ROW;
    FlexWrap wrap = FlexWrap::NOWRAP;
    FlexBasis basis = Width{Width::AUTO};
    f64 grow = 0;
    f64 shrink = 1;
};

} // namespace Vaev
