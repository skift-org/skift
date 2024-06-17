#pragma once

#include <karm-io/emit.h>

#include "length.h"
#include "percent.h"

namespace Web {

enum struct BoxSizing {
    CONTENT_BOX,
    BORDER_BOX,
};

struct Size {
    enum struct Type {
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

    Size() : type(AUTO), value(Length::ZERO) {
    }

    Size(Type type) : type(type), value(Length::ZERO) {
    }

    Size(Type type, PercentOr<Length> value) : type(type), value(value) {
    }

    Size(Percent value) : type(LENGTH), value(value) {
    }

    Size(Length value) : type(LENGTH), value(value) {
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
    BoxSizing boxSizing;
    Size width, height;
    Size minWidth, minHeight;
    Size maxWidth, maxHeight;
};

} // namespace Web
