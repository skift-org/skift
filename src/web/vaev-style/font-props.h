#pragma once

#include "font-face.h"

namespace Vaev::Style {

// https://drafts.csswg.org/css-fonts

// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc
struct AscentOverrideProp {
    Opt<Percent> value;

    static Str name() { return "ascent-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) const {
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
    Vec<String> value;

    static Str name() { return "font-family"; }

    static auto initial() { return Vec<String>{"serif"s}; }

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

using _FontProp = Union<
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
    UnicodeRangeProp

    /**/
    >;

struct FontProp : public _FontProp {
    using _FontProp::_FontProp;

    Str name() const {
        return visit([](auto const &p) {
            return p.name();
        });
    }

    void apply(FontFace &f) const {
        visit([&](auto const &p) {
            p.apply(f);
        });
    }

    void repr(Io::Emit &e) const {
        e("({}", name());
        visit([&](auto const &p) {
            e(" {}", p.value);
        });
        e(")");
    }
};

} // namespace Vaev::Style
