#pragma once

#include <karm-base/std.h>

namespace Web {

struct ZIndex {
    enum struct _Auto {
        AUTO,
    };

    using enum _Auto;

    bool auto_;
    isize value;

    ZIndex(_Auto) : auto_(true) {}

    ZIndex(isize value) : auto_(false), value(value) {}
};

} // namespace Web
