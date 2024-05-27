#pragma once

#include <web-unit/display.h>
#include <web-unit/visibility.h>

namespace Web::Css {

struct ComputedValues {
    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Unit::Display display = {Unit::Display::FLOW, Unit::Display::INLINE};
    isize order = 0;
    Unit::Visibility visibility = Unit::Visibility::VISIBLE;
};

} // namespace Web::Css
