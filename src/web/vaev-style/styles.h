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

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
struct BackgroundAttachmentProp {
    Vec<BackgroundAttachment> value = initial();

    static constexpr Str name() { return "background-attachment"; }

    static constexpr Array<BackgroundAttachment, 1> initial() {
        return {BackgroundAttachment::SCROLL};
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
struct BackgroundImageProp {
    Vec<Opt<Mime::Url>> value = initial();

    static constexpr Str name() { return "background-image"; }

    static Array<Opt<Mime::Url>, 1> initial() { return {NONE}; }

    void apply(Computed &) const {
        // computed.backgroundImage = value;
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
struct BorderColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(Computed &) const {
        // TODO
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-color
struct ColorStyleProp {
    Color value = initial();

    static constexpr Str name() { return "color"; }

    static constexpr Color initial() { return BLACK; }

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

// MARK: Margin ----------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-margin

struct MarginTopProp {
    MarginWidth value = initial();

    static Str name() { return "margin-top"; }

    static constexpr Length initial() { return Length{}; }

    void apply(Computed &c) const {
        c.margin.top = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<MarginWidth>(c));
        return Ok();
    }
};

struct MarginRightProp {
    MarginWidth value = initial();

    static Str name() { return "margin-right"; }

    static constexpr Length initial() { return Length{}; }

    void apply(Computed &c) const {
        c.margin.right = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<MarginWidth>(c));
        return Ok();
    }
};

struct MarginBottomProp {
    MarginWidth value = initial();

    static constexpr Str name() { return "margin-bottom"; }

    static constexpr Length initial() { return Length{}; }

    void apply(Computed &c) const {
        c.margin.bottom = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<MarginWidth>(c));
        return Ok();
    }
};

struct MarginLeftProp {
    MarginWidth value = initial();

    static Str name() { return "margin-left"; }

    static Length initial() { return Length{}; }

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

struct PaddingTopProp {
    PercentOr<Length> value = initial();

    static Str name() { return "padding-top"; }

    static Length initial() { return Length{}; }

    void apply(Computed &c) const {
        c.padding.top = value;
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
        c.padding.right = value;
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
        c.padding.bottom = value;
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
        c.padding.left = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<PercentOr<Length>>(c));
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

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
struct BoxSizingProp {
    BoxSizing value = initial();

    static constexpr Str name() { return "box-sizing"; }

    static constexpr BoxSizing initial() { return BoxSizing::CONTENT_BOX; }

    void apply(Computed &c) const {
        c.sizing.boxSizing = value;
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
        c.sizing.width = value;
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
        c.sizing.height = value;
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
        c.sizing.minWidth = value;
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
        c.sizing.minHeight = value;
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
        c.sizing.maxWidth = value;
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
        c.sizing.maxHeight = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// MARK: Prop ------------------------------------------------------------------

using _StyleProp = Union<
    BackgroundAttachmentProp,
    BackgroundColorProp,
    BackgroundImageProp,
    BackgroundPositionProp,
    BackgroundRepeatProp,
    ColorStyleProp,
    DisplayProp,

    // Margin
    MarginTopProp,
    MarginRightProp,
    MarginBottomProp,
    MarginLeftProp,

    // Overflow
    OverflowXProp,
    OverflowYProp,
    OverflowBlockProp,
    OverflowInlineProp,

    // Padding
    PaddingTopProp,
    PaddingRightProp,
    PaddingBottomProp,
    PaddingLeftProp,

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
