#pragma once

#include <karm-io/emit.h>
#include <karm-mime/url.h>
#include <vaev-base/color.h>

#include "computed.h"
#include "font-face.h"

// https://www.w3.org/TR/CSS22/propidx.html

namespace Vaev::Style {

// NOTE: This list should be kept alphabetically sorted.

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
struct BackgroundAttachmentProp {
    Vec<BackgroundAttachment> value;

    static Str name() { return "background-attachment"; }

    static auto initial() { return BackgroundAttachment::SCROLL; }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].attachment = value[i];
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
struct BackgroundColorProp {
    Vec<Color> value;

    static Str name() { return "background-color"; }

    static auto initial() { return TRANSPARENT; }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].paint = value[i];
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-image
struct BackgroundImageProp {
    Vec<Opt<Mime::Url>> value;

    static Str name() { return "background-image"; }

    static auto initial() { return NONE; }

    void apply(Computed &) const {
        // computed.backgroundImage = value;
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
struct BackgroundPositionProp {
    Vec<BackgroundPosition> value;

    static Str name() { return "background-position"; }

    static BackgroundPosition initial() { return {Percent{0}, Percent{0}}; }

    void apply(Computed &c) const {
        c.backgrounds.resize(max(c.backgrounds.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            c.backgrounds[i].position = value[i];
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
struct BackgroundRepeatProp {
    Vec<BackgroundRepeat> value;

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
    BorderCollapse value;

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
struct ColorProp {
    Color value;

    static Str name() { return "color"; }

    static auto initial() { return BLACK; }

    void apply(Computed &c) const {
        c.color = value;
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
struct DisplayProp {
    Display value;

    static Str name() { return "display"; }

    static Display initial() { return {Display::FLOW, Display::INLINE}; }

    void apply(Computed &s) const {
        s.display = value;
    }
};

// MARK: FontFace --------------------------------------------------------------
// https://drafts.csswg.org/css-fonts

// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc
struct AscentOverrideProp {
    Opt<Percent> value;

    static Str name() { return "ascent-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) {
        f.ascentOverride = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc

struct DescentOverrideProp {
    Opt<Percent> value;

    static Str name() { return "descent-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) const {
        f.descentOverride = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-display-desc
struct FontDisplayProp {
    FontDisplay value;

    static Str name() { return "font-display"; }

    static auto initial() { return FontDisplay::AUTO; }

    void apply(FontFace &f) const {
        f.display = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-family-desc
struct FontFamilyProp {
    Vec<Str> value;

    static Str name() { return "font-family"; }

    static auto initial() { return Vec<Str>{"serif"}; }

    void apply(FontFace &f) const {
        if (isEmpty(value))
            return;
        f.family = value[0];
    }
};

// https://drafts.csswg.org/css-fonts/#font-size-desc
struct FontStretchProp {
    FontStretch value;

    static Str name() { return "font-stretch"; }

    static auto initial() { return FontStretch::NORMAL; }

    void apply(FontFace &f) const {
        f.stretch = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-style-desc
struct FontStyleProp {
    FontStyle value;

    static Str name() { return "font-style"; }

    static auto initial() { return FontStyle::NORMAL; }

    void apply(FontFace &f) const {
        f.style = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-weight-desc
struct FontWeightProp {
    FontWeight value;

    static Str name() { return "font-weight"; }

    static auto initial() { return FontWeight::NORMAL; }

    void apply(FontFace &f) const {
        f.weight = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-feature-settings-prop
struct FontFeatureSettingsProp {
    Vec<FontFeature> value;

    static Str name() { return "font-feature-settings"; }

    static Vec<FontFeature> initial() { return {}; }

    void apply(FontFace &f) const {
        f.features = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-variation-settings-prop
struct FontVariationSettingsProp {
    Vec<FontVariation> value;

    static Str name() { return "font-variation-settings"; }

    static Vec<FontVariation> initial() { return {}; }

    void apply(FontFace &f) const {
        f.variations = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc
struct LineGapOverrideProp {
    Opt<Percent> value;

    static Str name() { return "line-gap-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) const {
        f.lineGapOverride = value;
    }
};

// https://drafts.csswg.org/css-fonts-5/#size-adjust-desc
struct SizeAdjustProp {
    Opt<Percent> value;

    static Str name() { return "size-adjust"; }

    static auto initial() {
        return NONE;
    }

    void apply(FontFace &f) const {
        f.sizeAdjust = value;
    }
};

// https://drafts.csswg.org/css-fonts/#src-desc
struct SrcProp {
    Vec<FontSource> value;

    static Str name() { return "src"; }

    static auto initial() {
        return Vec<FontSource>{};
    }

    void apply(FontFace &f) const {
        f.sources = value;
    }
};

// https://drafts.csswg.org/css-fonts/#unicode-range-desc
struct UnicodeRangeProp {
    Vec<Range<Rune>> value;

    static Str name() { return "unicode-range"; }

    static auto initial() {
        return Vec<Range<Rune>>{};
    }

    void apply(FontFace &f) const {
        f.unicodeRange = value;
    }
};

// MARK: Margin ----------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-margin

struct MarginTopProp {
    MarginWidth value;

    static Str name() { return "margin-top"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.top = value;
    }
};

struct MarginRightProp {
    MarginWidth value;

    static Str name() { return "margin-right"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.right = value;
    }
};

struct MarginBottomProp {
    MarginWidth value;

    static Str name() { return "margin-bottom"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.bottom = value;
    }
};

struct MarginLeftProp {
    MarginWidth value;

    static Str name() { return "margin-left"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.margin.left = value;
    }
};

// MARK: Padding ---------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-padding

struct PaddingTopProp {
    PercentOr<Length> value;

    static Str name() { return "padding-top"; }

    static Length initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.top = value;
    }
};

struct PaddingRightProp {
    PercentOr<Length> value;

    static Str name() { return "padding-right"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.right = value;
    }
};

struct PaddingBottomProp {
    PercentOr<Length> value;

    static Str name() { return "padding-bottom"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.bottom = value;
    }
};

struct PaddingLeftProp {
    PercentOr<Length> value;

    static Str name() { return "padding-left"; }

    static auto initial() { return Length::ZERO; }

    void apply(Computed &c) const {
        c.padding.left = value;
    }
};

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
struct BoxSizingProp {
    BoxSizing value;

    static Str name() { return "box-sizing"; }

    static auto initial() { return BoxSizing::CONTENT_BOX; }

    void apply(Computed &c) const {
        c.sizing.boxSizing = value;
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width

struct WidthProp {
    Size value;

    static Str name() { return "width"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.width = value;
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-height

struct HeightProp {
    Size value;

    static Str name() { return "height"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.height = value;
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-width

struct MinWidthProp {
    Size value;

    static Str name() { return "min-width"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.minWidth = value;
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-height

struct MinHeightProp {
    Size value;

    static Str name() { return "min-height"; }

    static auto initial() { return Size::AUTO; }

    void apply(Computed &c) const {
        c.sizing.minHeight = value;
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-width

struct MaxWidthProp {
    Size value;

    static Str name() { return "max-width"; }

    static auto initial() { return Size::NONE; }

    void apply(Computed &c) const {
        c.sizing.maxWidth = value;
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-height

struct MaxHeightProp {
    Size value;

    static Str name() { return "max-height"; }

    static auto initial() { return Size::NONE; }

    void apply(Computed &c) const {
        c.sizing.maxHeight = value;
    }
};

using _Prop = Union<
    BackgroundAttachmentProp,
    BackgroundColorProp,
    BackgroundImageProp,
    BackgroundPositionProp,
    BackgroundRepeatProp,
    ColorProp,
    DisplayProp,

    // FontFace
    AscentOverrideProp,
    DescentOverrideProp,
    FontDisplayProp,
    FontFamilyProp,
    FontStretchProp,
    FontStyleProp,
    FontWeightProp,
    FontFeatureSettingsProp,
    FontVariationSettingsProp,
    LineGapOverrideProp,
    SizeAdjustProp,
    SrcProp,
    UnicodeRangeProp,

    // Margin
    MarginTopProp,
    MarginRightProp,
    MarginBottomProp,
    MarginLeftProp,

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

struct Prop : public _Prop {
    using _Prop::_Prop;
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

    void apply(FontFace &f) const {
        visit([&](auto const &p) {
            if constexpr (requires { p.apply(f); })
                p.apply(f);
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