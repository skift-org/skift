#pragma once

#include <karm-io/emit.h>
#include <karm-mime/url.h>
#include <vaev-base/color.h>
#include <vaev-base/font.h>

#include "base.h"
#include "computed.h"
#include "values.h"

// https://www.w3.org/TR/CSS22/propidx.html

namespace Vaev::Style {

// MARK: Props -----------------------------------------------------------------

// NOTE: This list should be kept alphabetically sorted.

// MARK: Align -----------------------------------------------------------------
// https://drafts.csswg.org/css-align-3

// https://drafts.csswg.org/css-align-3/#propdef-align-content
struct AlignContentProp {
    Align value = initial();

    static constexpr Str name() { return "align-content"; }

    static constexpr Align initial() { return {}; }

    void apply(Computed &c) const {
        c.aligns.alignContent = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-content
struct JustifyContentProp {
    Align value = initial();

    static constexpr Str name() { return "justify-content"; }

    static constexpr Align initial() { return {}; }

    void apply(Computed &c) const {
        c.aligns.justifyContent = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-self
struct JustifySelfProp {
    Align value = initial();

    static constexpr Str name() { return "justify-self"; }

    static constexpr Align initial() { return {}; }

    void apply(Computed &c) const {
        c.aligns.justifySelf = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-self
struct AlignSelfProp {
    Align value = initial();

    static constexpr Str name() { return "align-self"; }

    static constexpr Align initial() { return {}; }

    void apply(Computed &c) const {
        c.aligns.alignSelf = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-items
struct JustifyItemsProp {
    Align value = initial();

    static constexpr Str name() { return "justify-items"; }

    static constexpr Align initial() { return {}; }

    void apply(Computed &c) const {
        c.aligns.justifyItems = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-items
struct AlignItemsProp {
    Align value = initial();

    static constexpr Str name() { return "align-items"; }

    static constexpr Align initial() { return {}; }

    void apply(Computed &c) const {
        c.aligns.alignItems = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
struct RowGapProp {
    PercentOr<Length> value = initial();

    static constexpr Str name() { return "row-gap"; }

    static constexpr PercentOr<Length> initial() { return {}; }

    void apply(Computed &c) const {
        c.gaps.y = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
struct ColumnGapProp {
    PercentOr<Length> value = initial();

    static constexpr Str name() { return "column-gap"; }

    static constexpr PercentOr<Length> initial() { return {}; }

    void apply(Computed &c) const {
        c.gaps.x = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

// MARK: Background ------------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
struct BackgroundAttachmentProp {
    Vec<BackgroundAttachment> value = initial();

    static constexpr Str name() { return "background-attachment"; }

    static constexpr Array<BackgroundAttachment, 1> initial() {
        return {BackgroundAttachment::SCROLL};
    }

    void apply(Computed &) const {
        // TODO
    }

    Res<> parse(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].attachment = value[i];
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
struct BackgroundColorProp {
    Vec<Color> value = initial();

    static constexpr Str name() { return "background-color"; }

    static constexpr Array<Color, 1> initial() { return {TRANSPARENT}; }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].fill = value[i];
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        eatWhitespace(c);
        while (not c.ended()) {
            value.pushBack(try$(parseValue<Color>(c)));
            eatWhitespace(c);
        }
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-image
struct BackgroundImageProp {
    Vec<Opt<Mime::Url>> value = initial();

    static constexpr Str name() { return "background-image"; }

    static Array<Opt<Mime::Url>, 1> initial() { return {NONE}; }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
struct BackgroundPositionProp {
    Vec<BackgroundPosition> value = initial();

    static constexpr Str name() { return "background-position"; }

    static constexpr Array<BackgroundPosition, 1> initial() {
        return {BackgroundPosition{Percent{0}, Percent{0}}};
    }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].position = value[i];
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
struct BackgroundRepeatProp {
    Vec<BackgroundRepeat> value = initial();

    static constexpr Str name() { return "background-repeat"; }

    static constexpr Array<BackgroundRepeat, 1> initial() {
        return {BackgroundRepeat::REPEAT};
    }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].repeat = value[i];
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-color
struct ColorProp {
    Color value = initial();

    static constexpr Str name() { return "color"; }

    static constexpr Color initial() { return BLACK; }

    static void inherit(Computed const &parent, Computed &child) {
        child.color = parent.color;
    }

    void apply(Computed &c) const {
        c.color = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
struct DisplayProp {
    Display value = initial();

    static constexpr Str name() { return "display"; }

    static constexpr Display initial() { return {Display::FLOW, Display::INLINE}; }

    void apply(Computed &s) const {
        s.display = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Display>(c));
        return Ok();
    }
};

// MARK: Borders ---------------------------------------------------------------

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse
struct BorderCollapseProp {
    BorderCollapse value = initial();

    static constexpr Str name() { return "border-collapse"; }

    static constexpr BorderCollapse initial() { return BorderCollapse::SEPARATE; }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderTopColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-top-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(Computed &c) const {
        c.borders.cow().top.color = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderRightColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-right-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(Computed &c) const {
        c.borders.cow().end.color = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderBottomColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-bottom-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(Computed &c) const {
        c.borders.cow().bottom.color = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderLeftColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-left-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(Computed &c) const {
        c.borders.cow().start.color = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderLeftStyleProp {
    BorderStyle value = initial();

    static constexpr Str name() { return "border-left-style"; }

    static constexpr BorderStyle initial() { return BorderStyle::NONE; }

    void apply(Computed &c) const {
        c.borders.cow().start.style = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderTopStyleProp {
    BorderStyle value = initial();

    static constexpr Str name() { return "border-top-style"; }

    static constexpr BorderStyle initial() { return BorderStyle::NONE; }

    void apply(Computed &c) const {
        c.borders.cow().top.style = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderRightStyleProp {
    BorderStyle value = initial();

    static constexpr Str name() { return "border-right-style"; }

    static constexpr BorderStyle initial() { return BorderStyle::NONE; }

    void apply(Computed &c) const {
        c.borders.cow().end.style = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderBottomStyleProp {
    BorderStyle value = initial();

    static constexpr Str name() { return "border-bottom-style"; }

    static constexpr BorderStyle initial() { return BorderStyle::NONE; }

    void apply(Computed &c) const {
        c.borders.cow().bottom.style = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderTopWidthProp {
    Length value = initial();

    static constexpr Str name() { return "border-top-width"; }

    static constexpr Length initial() { return Borders::MEDIUM; }

    void apply(Computed &c) const {
        c.borders.cow().top.width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Length>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderRightWidthProp {
    Length value = initial();

    static constexpr Str name() { return "border-right-width"; }

    static constexpr Length initial() { return Borders::MEDIUM; }

    void apply(Computed &c) const {
        c.borders.cow().end.width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Length>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderBottomWidthProp {
    Length value = initial();

    static constexpr Str name() { return "border-bottom-width"; }

    static constexpr Length initial() { return Borders::MEDIUM; }

    void apply(Computed &c) const {
        c.borders.cow().bottom.width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Length>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderLeftWidthProp {
    Length value = initial();

    static constexpr Str name() { return "border-left-width"; }

    static constexpr Length initial() { return Borders::MEDIUM; }

    void apply(Computed &c) const {
        c.borders.cow().start.width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Length>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusTopRight {
    Array<PercentOr<Length>, 2> value = {initial(), initial()};

    static constexpr Str name() { return "border-top-right-radius"; }

    static constexpr Length initial() { return Px{0}; }

    void apply(Computed &c) const {
        c.borders.cow().radii.a = value[0];
        c.borders.cow().radii.b = value[1];
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value[0] = try$(parseValue<PercentOr<Length>>(c));
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<PercentOr<Length>>(c));
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusTopLeft {
    Array<PercentOr<Length>, 2> value = {initial(), initial()};

    static constexpr Str name() { return "border-top-left-radius"; }

    static constexpr Length initial() { return Px{0}; }

    void apply(Computed &c) const {
        c.borders.cow().radii.c = value[0];
        c.borders.cow().radii.d = value[1];
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value[0] = try$(parseValue<PercentOr<Length>>(c));
        eatWhitespace(c);
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<PercentOr<Length>>(c));
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusBottomRight {
    Array<PercentOr<Length>, 2> value = {initial(), initial()};

    static constexpr Str name() { return "border-bottom-right-radius"; }

    static constexpr Length initial() { return Px{0}; }

    void apply(Computed &c) const {
        c.borders.cow().radii.e = value[0];
        c.borders.cow().radii.f = value[1];
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value[0] = try$(parseValue<PercentOr<Length>>(c));
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<PercentOr<Length>>(c));
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusBottomLeft {
    Array<PercentOr<Length>, 2> value = {initial(), initial()};

    static constexpr Str name() { return "border-bottom-left-radius"; }

    static constexpr Length initial() { return Px{0}; }

    void apply(Computed &c) const {
        c.borders.cow().radii.g = value[0];
        c.borders.cow().radii.h = value[1];
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value[0] = try$(parseValue<PercentOr<Length>>(c));
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<PercentOr<Length>>(c));
        }

        return Ok();
    }
};

// MARK: Flex ------------------------------------------------------------------

// https://www.w3.org/TR/css-flexbox-1/#flex-basis-property
struct FlexBasisProp {
    FlexBasis value = initial();

    static constexpr Str name() { return "flex-basis"; }

    static constexpr FlexBasis initial() { return Width{Width::AUTO}; }

    void apply(Computed &c) const {
        c.flex.cow().basis = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<FlexBasis>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-direction
struct FlexDirectionProp {
    FlexDirection value = initial();

    static constexpr Str name() { return "flex-direction"; }

    static constexpr FlexDirection initial() { return FlexDirection::ROW; }

    void apply(Computed &c) const {
        c.flex.cow().direction = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<FlexDirection>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#flex-grow-property
struct FlexGrowProp {
    f64 value = initial();

    static constexpr Str name() { return "flex-grow"; }

    static constexpr f64 initial() { return 0; }

    void apply(Computed &c) const {
        c.flex.cow().grow = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<f64>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-shrink
struct FlexShrinkProp {
    f64 value = initial();

    static constexpr Str name() { return "flex-shrink"; }

    static constexpr f64 initial() { return 1; }

    void apply(Computed &c) const {
        c.flex.cow().shrink = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<f64>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-wrap
struct FlexWrapProp {
    FlexWrap value = initial();

    static constexpr Str name() { return "flex-wrap"; }

    static constexpr FlexWrap initial() { return FlexWrap::NOWRAP; }

    void apply(Computed &c) const {
        c.flex.cow().wrap = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<FlexWrap>(c));
        return Ok();
    }
};

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
struct FontFamilyProp {
    Vec<String> value = initial();

    static constexpr Str name() { return "font-family"; }

    static Array<String, 1> initial() { return {"sans-serif"s}; }

    static void inherit(Computed const &parent, Computed &child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().families = parent.font->families;
    }

    void apply(Computed &c) const {
        c.font.cow().families = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        eatWhitespace(c);
        while (not c.ended()) {
            value.pushBack(try$(parseValue<String>(c)));
            eatWhitespace(c);
        }
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-weight-prop
struct FontWeightProp {
    FontWeight value = initial();

    static constexpr Str name() { return "font-weight"; }

    static constexpr FontWeight initial() { return FontWeight::NORMAL; }

    static void inherit(Computed const &parent, Computed &child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().weight = parent.font->weight;
    }

    void apply(Computed &c) const {
        c.font.cow().weight = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<FontWeight>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-width-prop
struct FontWidthProp {
    FontWidth value = initial();

    static constexpr Str name() { return "font-width"; }

    static constexpr FontWidth initial() { return FontWidth::NORMAL; }

    static void inherit(Computed const &parent, Computed &child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().width = parent.font->width;
    }

    void apply(Computed &c) const {
        c.font.cow().width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<FontWidth>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-style-prop
struct FontStyleProp {
    FontStyle value = initial();

    static constexpr Str name() { return "font-style"; }

    static constexpr FontStyle initial() { return FontStyle::NORMAL; }

    static void inherit(Computed const &parent, Computed &child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().style = parent.font->style;
    }

    void apply(Computed &c) const {
        c.font.cow().style = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<FontStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-size-prop
struct FontSizeProp {
    FontSize value = initial();

    static constexpr Str name() { return "font-size"; }

    static constexpr FontSize initial() { return FontSize::MEDIUM; }

    static void inherit(Computed const &parent, Computed &child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().size = parent.font->size;
    }

    void apply(Computed &c) const {
        c.font.cow().size = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<FontSize>(c));
        return Ok();
    }
};

// MARK: Margin ----------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-margin

struct MarginTopProp {
    Width value = initial();

    static Str name() { return "margin-top"; }

    static constexpr Width initial() { return Length{}; }

    void apply(Computed &c) const {
        c.margin.cow().top = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginRightProp {
    Width value = initial();

    static Str name() { return "margin-right"; }

    static constexpr Width initial() { return Length{}; }

    void apply(Computed &c) const {
        c.margin.cow().end = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginBottomProp {
    Width value = initial();

    static constexpr Str name() { return "margin-bottom"; }

    static constexpr Width initial() { return Length{}; }

    void apply(Computed &c) const {
        c.margin.cow().bottom = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginLeftProp {
    Width value = initial();

    static Str name() { return "margin-left"; }

    static Width initial() { return Length{}; }

    void apply(Computed &c) const {
        c.margin.cow().start = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginProp {
    Math::Insets<Width> value = initial();

    static Str name() { return "margin"; }

    static Math::Insets<Width> initial() { return {}; }

    void apply(Computed &c) const {
        c.margin.cow() = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Math::Insets<Width>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-color-4/#propdef-opacity

struct OpacityProp {
    Number value = initial();

    static Str name() { return "opacity"; }

    static f64 initial() { return 1; }

    void apply(Computed &c) const {
        c.opacity = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        auto maybePercent = parseValue<Percent>(c);
        if (maybePercent) {
            value = maybePercent.unwrap().value() / 100;
        } else {
            value = try$(parseValue<Number>(c));
        }
        return Ok();
    }
};

// MARK: Overflow --------------------------------------------------------------

// https://www.w3.org/TR/css-overflow/#overflow-control
struct OverflowXProp {
    Overflow value = initial();

    static Str name() { return "overflow-x"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(Computed &c) const {
        c.overflows.x = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-control
struct OverflowYProp {
    Overflow value = initial();

    static Str name() { return "overflow-y"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(Computed &c) const {
        c.overflows.y = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-block
struct OverflowBlockProp {
    Overflow value = initial();

    static Str name() { return "overflow-block"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(Computed &c) const {
        c.overflows.block = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-inline
struct OverflowInlineProp {
    Overflow value = initial();

    static Str name() { return "overflow-inline"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(Computed &c) const {
        c.overflows.inline_ = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// MARK: Padding ---------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-padding

struct PaddingTopProp {
    PercentOr<Length> value = initial();

    static Str name() { return "padding-top"; }

    static Length initial() { return Length{}; }

    void apply(Computed &c) const {
        c.padding.cow().top = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

struct PaddingRightProp {
    PercentOr<Length> value = initial();

    static Str name() { return "padding-right"; }

    static Length initial() { return Length{}; }

    void apply(Computed &c) const {
        c.padding.cow().end = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

struct PaddingBottomProp {
    PercentOr<Length> value = initial();

    static Str name() { return "padding-bottom"; }

    static Length initial() { return Length{}; }

    void apply(Computed &c) const {
        c.padding.cow().bottom = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

struct PaddingLeftProp {
    PercentOr<Length> value = initial();

    static Str name() { return "padding-left"; }

    static Length initial() { return {}; }

    void apply(Computed &c) const {
        c.padding.cow().start = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

struct PaddingProp {
    Math::Insets<PercentOr<Length>> value = initial();

    static Str name() { return "padding"; }

    static Math::Insets<PercentOr<Length>> initial() { return {}; }

    void apply(Computed &c) const {
        c.padding.cow() = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Math::Insets<PercentOr<Length>>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
struct OrderProp {
    Integer value = initial();

    static Str name() { return "order"; }

    static Integer initial() { return 0; }

    void apply(Computed &c) const {
        c.order = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Integer>(c));
        return Ok();
    }
};

// MARK: Positioning -----------------------------------------------------------

// https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
struct PositionProp {
    Position value = initial();

    static Str name() { return "position"; }

    static Position initial() { return Position::STATIC; }

    void apply(Computed &c) const {
        c.position = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Position>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
struct TopProp {
    Width value = initial();

    static Str name() { return "top"; }

    static Width initial() { return Width::AUTO; }

    void apply(Computed &c) const {
        c.offsets.cow().top = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
struct RightProp {
    Width value = initial();

    static Str name() { return "right"; }

    static Width initial() { return Width::AUTO; }

    void apply(Computed &c) const {
        c.offsets.cow().end = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
struct BottomProp {
    Width value = initial();

    static Str name() { return "bottom"; }

    static Width initial() { return Width::AUTO; }

    void apply(Computed &c) const {
        c.offsets.cow().bottom = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
struct LeftProp {
    Width value = initial();

    static Str name() { return "left"; }

    static Width initial() { return Width::AUTO; }

    void apply(Computed &c) const {
        c.offsets.cow().start = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
struct BoxSizingProp {
    BoxSizing value = initial();

    static constexpr Str name() { return "box-sizing"; }

    static constexpr BoxSizing initial() { return BoxSizing::CONTENT_BOX; }

    void apply(Computed &c) const {
        c.boxSizing = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        if (c.skip(Css::Token::ident("border-box")))
            value = BoxSizing::BORDER_BOX;
        else if (c.skip(Css::Token::ident("content-box")))
            value = BoxSizing::CONTENT_BOX;
        else
            return Error::invalidData("expected 'border-box' or 'content-box'");

        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width

struct WidthProp {
    Size value = initial();

    static constexpr Str name() { return "width"; }

    static constexpr Size initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.cow().width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-height

struct HeightProp {
    Size value = initial();

    static constexpr Str name() { return "height"; }

    static constexpr Size initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.cow().height = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-width

struct MinWidthProp {
    Size value = initial();

    static constexpr Str name() { return "min-width"; }

    static constexpr Size initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.cow().minWidth = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-height

struct MinHeightProp {
    Size value = initial();

    static constexpr Str name() { return "min-height"; }

    static constexpr Size initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.cow().minHeight = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-width

struct MaxWidthProp {
    Size value = initial();

    static constexpr Str name() { return "max-width"; }

    static constexpr Size initial() { return Size::NONE; }

    void apply(Computed &c) const {
        c.sizing.cow().maxWidth = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-height

struct MaxHeightProp {
    Size value = initial();

    static constexpr Str name() { return "max-height"; }

    static constexpr Size initial() { return Size::NONE; }

    void apply(Computed &c) const {
        c.sizing.cow().maxHeight = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// MARK: Style Property  -------------------------------------------------------

using _StyleProp = Union<
    // Align
    AlignContentProp,
    JustifyContentProp,
    JustifySelfProp,
    AlignSelfProp,
    JustifyItemsProp,
    AlignItemsProp,

    RowGapProp,
    ColumnGapProp,

    // Background
    BackgroundAttachmentProp,
    BackgroundColorProp,
    BackgroundImageProp,
    BackgroundPositionProp,
    BackgroundRepeatProp,
    ColorProp,
    DisplayProp,

    // Borders
    BorderTopColorProp,
    BorderRightColorProp,
    BorderBottomColorProp,
    BorderLeftColorProp,

    BorderTopWidthProp,
    BorderRightWidthProp,
    BorderBottomWidthProp,
    BorderLeftWidthProp,

    BorderTopStyleProp,
    BorderRightStyleProp,
    BorderBottomStyleProp,
    BorderLeftStyleProp,

    BorderRadiusTopRight,
    BorderRadiusTopLeft,
    BorderRadiusBottomRight,
    BorderRadiusBottomLeft,

    // Flex
    FlexBasisProp,
    FlexDirectionProp,
    FlexGrowProp,
    FlexShrinkProp,
    FlexWrapProp,

    // Font
    FontFamilyProp,
    FontWeightProp,
    FontWidthProp,
    FontStyleProp,
    FontSizeProp,

    // Margin
    MarginTopProp,
    MarginRightProp,
    MarginBottomProp,
    MarginLeftProp,
    MarginProp,

    // Overflow
    OverflowXProp,
    OverflowYProp,
    OverflowBlockProp,
    OverflowInlineProp,

    OpacityProp,

    // Padding
    PaddingTopProp,
    PaddingRightProp,
    PaddingBottomProp,
    PaddingLeftProp,
    PaddingProp,

    // Positioning
    PositionProp,
    TopProp,
    RightProp,
    BottomProp,
    LeftProp,

    // Sizing
    BoxSizingProp,
    WidthProp,
    HeightProp,
    MinWidthProp,
    MinHeightProp,
    MaxWidthProp,
    MaxHeightProp

    /**/
    >;

enum struct Important {
    NO,
    YES,
    VERY,
};

struct StyleProp : public _StyleProp {
    using _StyleProp::_StyleProp;
    Important important = Important::NO;

    static constexpr Array LEGACY_ALIAS = {
        // https://drafts.csswg.org/css-align-3/#gap-legacy
        Cons<Str>{"grid-row-gap", "row-gap"},
        Cons<Str>{"grid-column-gap", "column-gap"},
        Cons<Str>{"grid-gap", "gap"},
    };

    Str name() const {
        return visit([](auto const &p) {
            return p.name();
        });
    }

    void inherit(Computed const &parent, Computed &child) const {
        visit([&](auto const &p) {
            if constexpr (requires { p.inherit(parent, child); })
                p.inherit(parent, child);
        });
    }

    void apply(Computed &c) const {
        visit([&](auto const &p) {
            if constexpr (requires { p.apply(c); })
                p.apply(c);
        });
    }

    void repr(Io::Emit &e) const {
        e("({}", name());
        visit([&](auto const &p) {
            e(" {}", p.value);
            if (important == Important::YES)
                e(" !important");
        });
        e(")");
    }
};

} // namespace Vaev::Style
