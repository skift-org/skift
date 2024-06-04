#pragma once

#include <karm-gfx/color.h>
#include <web-types/background.h>
#include <web-types/borders.h>
#include <web-types/display.h>
#include <web-types/flex.h>
#include <web-types/float.h>
#include <web-types/visibility.h>

namespace Web::Cssom {

struct Style {
    Vec<Types::Background> backgrounds;
    Types::Borders borders;

    // CSS Box Sizing Module Level 4
    // https://drafts.csswg.org/css-sizing-4/
    Opt<Types::Length> width;
    Opt<Types::Length> height;
    Opt<Types::Length> minWidth;
    Opt<Types::Length> minHeight;
    Opt<Types::Length> maxWidth;
    Opt<Types::Length> maxHeight;

    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Types::Display display = {Types::Display::FLOW, Types::Display::INLINE};
    isize order = 0;
    Types::Visibility visibility = Types::Visibility::VISIBLE;

    // CSS Flexible Box Layout Module Level 1
    // https://www.w3.org/TR/css-flexbox-1
    Types::FlexDirection flexDirection = Types::FlexDirection::ROW;
    Types::FlexWrap flexWrap = Types::FlexWrap::NOWRAP;
    f64 flexGrow = 0;
    f64 flexShrink = 1;

    Types::Float float_ = Types::Float::NONE;
    Types::Clear clear = Types::Clear::NONE;
};

} // namespace Web::Cssom
