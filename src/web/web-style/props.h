#pragma once

#include <karm-mime/url.h>
#include <web-css/colors.h>

#include "computed.h"
#include "font-face.h"

// https://www.w3.org/TR/CSS22/propidx.html

namespace Web::Style {

// NOTE: This list should be kept alphabetically sorted.

// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc

struct AscentOverrideProp {
    Opt<Percent> value;

    static Str name() { return "ascent-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) {
        f.ascentOverride = value;
    }
};

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
    Vec<Gfx::Color> value;

    static Str name() { return "background-color"; }

    static auto initial() { return Css::TRANSPARENT; }

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
    Gfx::Color value;

    static Str name() { return "border-color"; }

    static auto initial() { return Css::BLACK; }

    void apply(Computed &) const {
        // TODO
    }
};

// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc

struct DescentOverrideProp {
    Opt<Percent> value;

    static Str name() { return "descent-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) {
        f.descentOverride = value;
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

// https://drafts.csswg.org/css-fonts/#font-display-desc
struct FontDisplayProp {
    FontDisplay value;

    static Str name() { return "font-display"; }

    static auto initial() { return FontDisplay::AUTO; }

    void apply(FontFace &f) {
        f.display = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-family-desc
struct FontFamilyProp {
    Vec<Str> value;

    static Str name() { return "font-family"; }

    static auto initial() { return Vec<Str>{"serif"}; }

    void apply(FontFace &f) {
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

    void apply(FontFace &f) {
        f.stretch = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-style-desc
struct FontStyleProp {
    FontStyle value;

    static Str name() { return "font-style"; }

    static auto initial() { return FontStyle::NORMAL; }

    void apply(FontFace &f) {
        f.style = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-weight-desc
struct FontWeightProp {
    FontWeight value;

    static Str name() { return "font-weight"; }

    static auto initial() { return FontWeight::NORMAL; }

    void apply(FontFace &f) {
        f.weight = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-feature-settings-prop
struct FontFeatureSettingsProp {
    Vec<FontFeature> value;

    static Str name() { return "font-feature-settings"; }

    static Vec<FontFeature> initial() { return {}; }

    void apply(FontFace &f) {
        f.features = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-variation-settings-prop
struct FontVariationSettingsProp {
    Vec<FontVariation> value;

    static Str name() { return "font-variation-settings"; }

    static Vec<FontVariation> initial() { return {}; }

    void apply(FontFace &f) {
        f.variations = value;
    }
};

// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc
struct LineGapOverrideProp {
    Opt<Percent> value;

    static Str name() { return "line-gap-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) {
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

using _Prop = Union<
    AscentOverrideProp,
    BackgroundAttachmentProp,
    BackgroundColorProp,
    BackgroundImageProp,
    BackgroundPositionProp,
    BackgroundRepeatProp,
    DisplayProp,
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
    UnicodeRangeProp

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
};

} // namespace Web::Style
