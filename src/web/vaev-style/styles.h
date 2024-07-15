#pragma once

#include <karm-io/emit.h>
#include <karm-mime/url.h>
#include <vaev-base/color.h>

#include "base.h"
#include "computed.h"
#include "props.h"
#include "values.h"

// https://www.w3.org/TR/CSS22/propidx.html

namespace Vaev::Style {

// MARK: Props -----------------------------------------------------------------

// NOTE: This list should be kept alphabetically sorted.

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
struct BackgroundAttachmentStyleProp {
    Vec<BackgroundAttachment> value{};

    static Str name() { return "background-attachment"; }

    static auto initial() { return BackgroundAttachment::SCROLL; }

    Res<> parse(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].attachment = value[i];
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
struct BackgroundColorStyleProp {
    Vec<Color> value{};

    static Str name() { return "background-color"; }

    static auto initial() { return TRANSPARENT; }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].paint = value[i];
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
struct BackgroundImageStyleProp {
    Vec<Opt<Mime::Url>> value{};

    static Str name() { return "background-image"; }

    static auto initial() { return NONE; }

    void apply(Computed &) const {
        // computed.backgroundImage = value;
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
struct BackgroundPositionStyleProp {
    Vec<BackgroundPosition> value{};

    static Str name() { return "background-position"; }

    static BackgroundPosition initial() { return {Percent{0}, Percent{0}}; }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].position = value[i];
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
struct BackgroundRepeatStyleProp {
    Vec<BackgroundRepeat> value{};

    static Str name() { return "background-repeat"; }

    static BackgroundRepeat initial() { return BackgroundRepeat::REPEAT; }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].repeat = value[i];
    }
};

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse
struct BorderCollapseProp {
    BorderCollapse value{};

    static Str name() { return "border-collapse"; }

    static auto initial() { return BorderCollapse::SEPARATE; }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderColorProp {
    Color value;

    static Str name() { return "border-color"; }

    static auto initial() { return BLACK; }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-color
struct ColorStyleStyleProp {
    Color value;

    static Str name() { return "color"; }

    static auto initial() { return BLACK; }

    void apply(Computed &c) const {
        c.color = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
struct DisplayStyleProp {
    Display value;

    static Str name() { return "display"; }

    static Display initial() { return {Display::FLOW, Display::INLINE}; }

    void apply(Computed &s) const {
        s.display = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Display>(c));
        return Ok();
    }
};

// MARK: Margin ----------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-margin

struct MarginTopStyleProp {
    MarginWidth value;

    static Str name() { return "margin-top"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.top = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<MarginWidth>(c));
        return Ok();
    }
};

struct MarginRightStyleProp {
    MarginWidth value;

    static Str name() { return "margin-right"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.right = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<MarginWidth>(c));
        return Ok();
    }
};

struct MarginBottomStyleProp {
    MarginWidth value;

    static Str name() { return "margin-bottom"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.bottom = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<MarginWidth>(c));
        return Ok();
    }
};

struct MarginLeftStyleProp {
    MarginWidth value;

    static Str name() { return "margin-left"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.left = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<MarginWidth>(c));
        return Ok();
    }
};

// MARK: Padding ---------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-padding

struct PaddingTopStyleProp {
    PercentOr<Length> value;

    static Str name() { return "padding-top"; }

    static Length initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.top = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

struct PaddingRightStyleProp {
    PercentOr<Length> value;

    static Str name() { return "padding-right"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.right = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

struct PaddingBottomStyleProp {
    PercentOr<Length> value;

    static Str name() { return "padding-bottom"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.bottom = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

struct PaddingLeftStyleProp {
    PercentOr<Length> value;

    static Str name() { return "padding-left"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.left = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
struct OrderProp {
    Integer value;

    static Str name() { return "order"; }

    static auto initial() { return 0; }

    void apply(Computed &c) const {
        c.order = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Integer>(c));
        return Ok();
    }
};

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
struct BoxSizingStyleProp {
    BoxSizing value;

    static Str name() { return "box-sizing"; }

    static auto initial() { return BoxSizing::CONTENT_BOX; }

    void apply(Computed &c) const {
        c.sizing.boxSizing = value;
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width

struct WidthStyleProp {
    Size value;

    static Str name() { return "width"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-height

struct HeightStyleProp {
    Size value;

    static Str name() { return "height"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.height = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-width

struct MinWidthStyleProp {
    Size value;

    static Str name() { return "min-width"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.minWidth = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-height

struct MinHeightStyleProp {
    Size value;

    static Str name() { return "min-height"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.minHeight = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-width

struct MaxWidthStyleProp {
    Size value;

    static Str name() { return "max-width"; }

    static auto initial() { return Size::NONE; }

    void apply(Computed &c) const {
        c.sizing.maxWidth = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-height

struct MaxHeightStyleProp {
    Size value;

    static Str name() { return "max-height"; }

    static auto initial() { return Size::NONE; }

    void apply(Computed &c) const {
        c.sizing.maxHeight = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// MARK: Prop ------------------------------------------------------------------

using _StyleProp = Union<
    BackgroundAttachmentStyleProp,
    BackgroundColorStyleProp,
    BackgroundImageStyleProp,
    BackgroundPositionStyleProp,
    BackgroundRepeatStyleProp,
    ColorStyleStyleProp,
    DisplayStyleProp,

    // Margin
    MarginTopStyleProp,
    MarginRightStyleProp,
    MarginBottomStyleProp,
    MarginLeftStyleProp,

    // Padding
    PaddingTopStyleProp,
    PaddingRightStyleProp,
    PaddingBottomStyleProp,
    PaddingLeftStyleProp,

    // Sizing
    BoxSizingStyleProp,
    WidthStyleProp,
    HeightStyleProp,
    MinWidthStyleProp,
    MinHeightStyleProp,
    MaxWidthStyleProp,
    MaxHeightStyleProp

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

    Str name() const {
        return visit([](auto const &p) {
            return p.name();
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
