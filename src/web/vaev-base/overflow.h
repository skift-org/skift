#pragma once

namespace Vaev {

// https://www.w3.org/TR/css-overflow/#overflow-control
enum struct Overflow {
    VISIBLE,
    HIDDEN,
    SCROLL,
    AUTO,
};

struct Overflows {
    Overflow x = Overflow::VISIBLE;
    Overflow y = Overflow::VISIBLE;
    Overflow block = Overflow::VISIBLE;
    Overflow inline_ = Overflow::VISIBLE;
};

} // namespace Vaev
