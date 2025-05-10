#pragma once

#include <karm-base/cow.h>
#include <vaev-base/align.h>
#include <vaev-base/background.h>
#include <vaev-base/baseline.h>
#include <vaev-base/basic-shape.h>
#include <vaev-base/borders.h>
#include <vaev-base/break.h>
#include <vaev-base/color.h>
#include <vaev-base/display.h>
#include <vaev-base/flex.h>
#include <vaev-base/float.h>
#include <vaev-base/font.h>
#include <vaev-base/insets.h>
#include <vaev-base/line.h>
#include <vaev-base/numbers.h>
#include <vaev-base/outline.h>
#include <vaev-base/overflow.h>
#include <vaev-base/sizing.h>
#include <vaev-base/table.h>
#include <vaev-base/text.h>
#include <vaev-base/visibility.h>
#include <vaev-base/z-index.h>

#include "css/parser.h"

namespace Vaev::Style {

struct ComputedStyle {
    static ComputedStyle const& initial();

    Gfx::Color color;
    Number opacity;
    String content = ""s;

    AlignProps aligns;
    Cow<Gaps> gaps;

    Cow<BackgroundProps> backgrounds;
    Cow<BorderProps> borders;
    Cow<Margin> margin = makeCow<Margin>(Width(CalcValue<PercentOr<Length>>(Length(0_au)))); // FIXME
    Cow<Outline> outline;
    Cow<Padding> padding = makeCow<Padding>(Length(0_au)); // FIXME
    BoxSizing boxSizing;
    Cow<SizingProps> sizing;
    Overflows overflows;
    Opt<BasicShape> clip;

    // CSS Inline Layout Module Level 3
    // https://drafts.csswg.org/css-inline-3/
    Cow<Baseline> baseline;

    // 9.3 Positioning schemes
    // https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
    Position position;
    Cow<Offsets> offsets = makeCow<Offsets>(Width(Keywords::AUTO)); // FIXME

    // CSS Writing Modes Level 3
    // https://www.w3.org/TR/css-writing-modes-3
    WritingMode writingMode;
    Direction direction;

    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Display display;
    Integer order;
    Visibility visibility;
    // https://w3.org/TR/css-tables-3/#table-structure
    Cow<TableProps> table;

    // CSS Fonts Module Level 4
    // https://www.w3.org/TR/css-fonts-4/
    Cow<FontProps> font;
    Cow<TextProps> text;

    Cow<FlexProps> flex;
    Cow<BreakProps> break_;

    Cow<Map<String, Css::Content>> variables;

    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;

    // https://drafts.csswg.org/css2/#z-index
    ZIndex zIndex = Keywords::AUTO;

    void inherit(ComputedStyle const& parent);

    void repr(Io::Emit& e) const;

    void setCustomProp(Str varName, Css::Content value);

    Css::Content getCustomProp(Str varName) const;
};

} // namespace Vaev::Style
