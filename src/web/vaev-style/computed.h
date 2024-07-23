#pragma once

#include <karm-base/cow.h>
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
    static Computed const &initial();

    Color color;
    Number opacity;

    Vec<Background> backgrounds;
    Cow<Borders> borders;
    Cow<Margin> margin;
    Cow<Padding> padding;
    BoxSizing boxSizing;
    Cow<Sizing> sizing;
    Overflows overflows;

    // 9.3 Positioning schemes
    // https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
    Position position;
    Cow<Offsets> offsets;

    // CSS Writing Modes Level 3
    // https://www.w3.org/TR/css-writing-modes-3
    WritingMode writingMode;
    Direction direction;

    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Display display;
    Integer order;
    Visibility visibility;

    // CSS Fonts Module Level 4
    // https://www.w3.org/TR/css-fonts-4/
    Cow<Font> font;

    Cow<Flex> flex;

    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;

    void inherit(Computed const &parent) {
        color = parent.color;
        font = parent.font;
    }
};

} // namespace Vaev::Style
