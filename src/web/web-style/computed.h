#pragma once

#include <karm-gfx/color.h>
#include <web-base/background.h>
#include <web-base/borders.h>
#include <web-base/color.h>
#include <web-base/display.h>
#include <web-base/flex.h>
#include <web-base/float.h>
#include <web-base/insets.h>
#include <web-base/sizing.h>
#include <web-base/visibility.h>

namespace Web::Style {

struct Computed {
    Color color;

    Vec<Background> backgrounds;
    Borders borders;
    Margin margin;
    Padding padding;
    Sizing sizing;

    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Display display = {Display::FLOW, Display::INLINE};
    isize order = 0;
    Visibility visibility = Visibility::VISIBLE;

    Flex flex;

    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;
};

} // namespace Web::Style
