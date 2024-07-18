#pragma once

#include <karm-gfx/color.h>
#include <vaev-base/background.h>
#include <vaev-base/borders.h>
#include <vaev-base/color.h>
#include <vaev-base/display.h>
#include <vaev-base/flex.h>
#include <vaev-base/float.h>
#include <vaev-base/font.h>
#include <vaev-base/insets.h>
#include <vaev-base/numbers.h>
#include <vaev-base/overflow.h>
#include <vaev-base/sizing.h>
#include <vaev-base/visibility.h>

namespace Vaev::Style {

struct Computed {
    Color color;

    Vec<Background> backgrounds;
    Borders borders;
    Margin margin;
    Padding padding;
    Sizing sizing;
    Overflows overflows;

    // CSS Writing Modes Level 3
    // https://www.w3.org/TR/css-writing-modes-3
    WritingMode writingMode = WritingMode::HORIZONTAL_TB;
    Direction direction = Direction::LTR;

    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Display display = {Display::FLOW, Display::INLINE};
    Integer order = 0;
    Visibility visibility = Visibility::VISIBLE;

    // CSS Fonts Module Level 4
    // https://www.w3.org/TR/css-fonts-4/
    Vec<String> fontFamilies;
    FontWeight fontWeigh;
    FontWidth fontWidth;
    FontStyle fontStyle;
    FontSize fontsize;

    Flex flex;

    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;
};

} // namespace Vaev::Style
