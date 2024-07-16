#pragma once

#include <karm-base/std.h>

namespace Vaev {

struct Ratio {
    f64 num;
    f64 den = 1.0;

    constexpr f64 eval() const {
        return num / den;
    }
};

} // namespace Vaev
