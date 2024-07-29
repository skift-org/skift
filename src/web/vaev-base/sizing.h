#pragma once

#include <karm-io/emit.h>

#include "length.h"
#include "percent.h"
#include "writing.h"

namespace Vaev {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
enum struct BoxSizing : u8 {
    CONTENT_BOX,
    BORDER_BOX,
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
struct Size {
    enum struct Type : u8 {
        NONE,
        AUTO,
        LENGTH,
        MIN_CONTENT,
        MAX_CONTENT,
        FIT_CONTENT,
    };

    using enum Type;

    Type type;
    PercentOr<Length> value;

    constexpr Size() : type(AUTO), value(Length{}) {
    }

    constexpr Size(Type type) : type(type), value(Length{}) {
    }

    constexpr Size(Type type, PercentOr<Length> value) : type(type), value(value) {
    }

    constexpr Size(Percent value) : type(LENGTH), value(value) {
    }

    constexpr Size(Length value) : type(LENGTH), value(value) {
    }

    bool operator==(Type type) const {
        return this->type == type;
    }

    void repr(Io::Emit &e) const {
        switch (type) {
        case Type::NONE:
            e("none");
            break;

        case Type::AUTO:
            e("auto");
            break;

        case Type::LENGTH:
            e("{}", value);
            break;

        case Type::MIN_CONTENT:
            e("min-content");
            break;

        case Type::MAX_CONTENT:
            e("max-content");
            break;

        case Type::FIT_CONTENT:
            e("fit-content");
            break;

        default:
            e("unknown");
            break;
        }
    }
};

struct Sizing {
    Size width, height;
    Size minWidth, minHeight;
    Size maxWidth, maxHeight;

    Size &size(Axis axis) {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size const size(Axis axis) const {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size &minSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Size const minSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Size &maxSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    Size const maxSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }
};

} // namespace Vaev
