module;

#include <karm-gfx/color.h>
#include <karm-gfx/font.h>
#include <karm-math/au.h>

export module Vaev.Engine:style.specified;

import Karm.Core;
import :css;
import :values;

using namespace Karm;

namespace Vaev::Style {

struct TransformProps {
    Transform transform = Keywords::NONE;
    TransformOrigin origin = {
        .xOffset = CalcValue<PercentOr<Length>>{Percent{0}},
        .yOffset = CalcValue<PercentOr<Length>>{Percent{0}},
    };
    TransformBox box = Keywords::BORDER_BOX;

    bool has() const {
        return transform != Keywords::NONE;
    }
};

// https://www.w3.org/TR/css-cascade/#specified
export struct SpecifiedValues {
    static SpecifiedValues const& initial();

    SpecifiedValues() : fontFace(Gfx::Fontface::fallback()) {}

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
    Cow<TransformProps> transform;
    // https://w3.org/TR/css-tables-3/#table-structure
    Cow<TableProps> table;

    // CSS Fonts Module Level 4
    // https://www.w3.org/TR/css-fonts-4/
    Cow<FontProps> font;
    Cow<TextProps> text;

    Cow<FlexProps> flex;
    Cow<BreakProps> break_;

    Cow<Map<Symbol, Css::Content>> variables;

    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;

    // https://drafts.csswg.org/css2/#z-index
    ZIndex zIndex = Keywords::AUTO;

    Cow<SVGProps> svg;

    // ---------- Computed Style ---------------------

    Rc<Gfx::Fontface> fontFace;

    void inherit(SpecifiedValues const& parent) {
        color = parent.color;
        font = parent.font;
        text = parent.text;
        variables = parent.variables;
        visibility = parent.visibility;

        // FIXME: this is not clean and should be targeted by the styling refactor
        svg.cow().fillOpacity = parent.svg->fillOpacity;
        svg.cow().strokeWidth = parent.svg->strokeWidth;
        svg.cow().fill = parent.svg->fill;
        svg.cow().stroke = parent.svg->stroke;
    }

    void setCustomProp(Str varName, Css::Content value) {
        setCustomProp(Symbol::from(varName), value);
    }

    void setCustomProp(Symbol varName, Css::Content value) {
        variables.cow().put(varName, value);
    }

    Css::Content getCustomProp(Str varName) const {
        auto value = variables->access(Symbol::from(varName));
        if (value)
            return *value;
        return {};
    }

    void repr(Io::Emit& e) const {
        e("(computed");
        e(" color: {}", color);
        e(" opacity: {}", opacity);
        e(" aligns: {}", aligns);
        e(" gaps: {}", gaps);
        e(" backgrounds: {}", backgrounds);
        e(" baseline: {}", baseline);
        e(" borders: {}", borders);
        e(" margin: {}", margin);
        e(" padding: {}", padding);
        e(" boxSizing: {}", boxSizing);
        e(" sizing: {}", sizing);
        e(" overflows: {}", overflows);
        e(" position: {}", position);
        e(" offsets: {}", offsets);
        e(" writingMode: {}", writingMode);
        e(" direction: {}", direction);
        e(" display: {}", display);
        e(" order: {}", order);
        e(" visibility: {}", visibility);
        e(" table: {}", table);
        e(" font: {}", font);
        e(" text: {}", text);
        e(" flex: {}", flex);
        e(" break: {}", break_);
        e(" float: {}", float_);
        e(" clear: {}", clear);
        e(" svg: {}", svg);
        e(" zIndex: {}", zIndex);
        e(" variables: {}", variables);
        e(")");
    }
};

} // namespace Vaev::Style
