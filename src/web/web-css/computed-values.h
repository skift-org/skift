#pragma once

#include <web-unit/display.h>
#include <web-unit/flex.h>
#include <web-unit/float.h>
#include <web-unit/visibility.h>

namespace Web::Css {

struct ComputedValues {
    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Unit::Display display = {Unit::Display::FLOW, Unit::Display::INLINE};
    isize order = 0;
    Unit::Visibility visibility = Unit::Visibility::VISIBLE;

    // CSS Flexible Box Layout Module Level 1
    // https://www.w3.org/TR/css-flexbox-1
    Unit::FlexDirection flexDirection = Unit::FlexDirection::ROW;
    Unit::FlexWrap flexWrap = Unit::FlexWrap::NOWRAP;
    f64 flexGrow = 0;
    f64 flexShrink = 1;

    Unit::Float float_ = Unit::Float::NONE;
    Unit::Clear clear = Unit::Clear::NONE;
};

} // namespace Web::Css
