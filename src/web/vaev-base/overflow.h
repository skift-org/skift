#pragma once

#include <karm-io/emit.h>

namespace Vaev {

// https://www.w3.org/TR/css-overflow/#overflow-control
enum struct Overflow {
    VISIBLE,
    HIDDEN,
    SCROLL,
    AUTO,

    _LEN
};

struct Overflows {
    Overflow x = Overflow::VISIBLE;
    Overflow y = Overflow::VISIBLE;
    Overflow block = Overflow::VISIBLE;
    Overflow inline_ = Overflow::VISIBLE;

    void repr(Io::Emit& e) const {
        e("(overflows");
        e(" x={}", x);
        e(" y={}", y);
        e(" block={}", block);
        e(" inline={}", inline_);
        e(")");
    }
};

} // namespace Vaev
