#pragma once

#include <karm-gfx/color.h>
#include <web-base/background.h>
#include <web-base/borders.h>
#include <web-base/color.h>
#include <web-base/display.h>
#include <web-base/flex.h>
#include <web-base/float.h>
#include <web-base/visibility.h>

namespace Web::Style {

struct Computed {
    Color color;

    Vec<Background> backgrounds;
    Borders borders;

    // CSS Box Sizing Module Level 4
    // https://drafts.csswg.org/css-sizing-4/
    Opt<Length> width;
    Opt<Length> height;
    Opt<Length> minWidth;
    Opt<Length> minHeight;
    Opt<Length> maxWidth;
    Opt<Length> maxHeight;

    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Display display = {Display::FLOW, Display::INLINE};
    isize order = 0;
    Visibility visibility = Visibility::VISIBLE;

    // CSS Flexible Box Layout Module Level 1
    // https://www.w3.org/TR/css-flexbox-1
    FlexDirection flexDirection = FlexDirection::ROW;
    FlexWrap flexWrap = FlexWrap::NOWRAP;
    f64 flexGrow = 0;
    f64 flexShrink = 1;

    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;
};

} // namespace Web::Style
