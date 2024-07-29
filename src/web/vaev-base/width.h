#pragma once

#include "length.h"
#include "percent.h"

namespace Vaev {

struct Width {
    enum struct Type {
        AUTO,
        VALUE,
    };

    using enum Type;

    Type type;
    PercentOr<Length> value;

    constexpr Width(Type type)
        : type(type) {
    }

    constexpr Width(PercentOr<Length> percent = {})
        : type(Type::VALUE), value(percent) {
    }

    constexpr Width(Length length)
        : type(Type::VALUE), value(length) {
    }

    void repr(Io::Emit &e) const {
        if (type == Type::AUTO) {
            e("auto");
        } else {
            e("{}", value);
        }
    }

    bool operator==(Type t) const {
        return type == t;
    }
};

} // namespace Vaev
