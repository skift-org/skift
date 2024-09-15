#pragma once

#include "length.h"
#include "percent.h"
#include "vaev-base/calc.h"

namespace Vaev {

struct Width {
    enum struct Type {
        AUTO,
        VALUE,
    };

    using enum Type;

    Type type;
    CalcValue<PercentOr<Length>> value;

    constexpr Width(Type type)
        : type(type) {
    }

    Width(CalcValue<PercentOr<Length>> percent = {})
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
