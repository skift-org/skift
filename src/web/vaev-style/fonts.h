#pragma once

#include <karm-mime/url.h>
#include <vaev-base/angle.h>
#include <vaev-base/percent.h>
#include <vaev-css/parser.h>

#include "values.h"

namespace Vaev::Style {

// MARK: Values ----------------------------------------------------------------

enum struct FontDisplay {
    AUTO,
    BLOCK,
    SWAP,
    FALLBACK,
    OPTIONAL,

    _LEN,
};

struct FontWidth {
    enum struct _Named {
        ULTRA_CONDENSED = 500,
        EXTRA_CONDENSED = 625,
        CONDENSED = 750,
        SEMI_CONDENSED = 875,
        NORMAL = 1000,
        SEMI_EXPANDED = 1125,
        EXPANDED = 1250,
        EXTRA_EXPANDED = 1500,
        ULTRA_EXPANDED = 2000,

        _LEN,
    };

    static Percent toPercent(_Named stretch) {
        return Percent{static_cast<int>(stretch) / 10.0};
    }

    using enum _Named;

    Percent _val;

    FontWidth()
        : FontWidth(NORMAL) {
    }

    FontWidth(_Named named)
        : _val(toPercent(named)) {
    }

    FontWidth(Percent value)
        : _val(value) {
    }

    auto val() const {
        return _val;
    }

    void repr(Io::Emit &e) const {
        e("{}", _val);
    }
};

struct FontStyle {
    enum struct _Named {
        NORMAL,
        ITALIC,
        OBLIQUE,

        _LEN,
    };

    using enum _Named;

    _Named val;
    Range<Angle> angles;

    FontStyle()
        : FontStyle(NORMAL) {
    }

    FontStyle(_Named named)
        : val(named) {
    }

    FontStyle(Angle angle)
        : val(OBLIQUE), angles(Range<Angle>::fromStartEnd(angle, angle)) {}

    FontStyle(Angle start, Angle end)
        : val(OBLIQUE), angles(Range<Angle>::fromStartEnd(start, end)) {}

    void repr(Io::Emit &e) const {
        e("{}", val);
        if (val == OBLIQUE)
            e(" {}", angles);
    }
};

struct FontWeight {
    enum struct _Named {
        THIN = 100,
        EXTRA_LIGHT = 200,
        LIGHT = 300,
        NORMAL = 400,
        MEDIUM = 500,
        SEMI_BOLD = 600,
        BOLD = 700,
        EXTRA_BOLD = 800,
        BLACK = 900,

        _LEN,
    };

    using enum _Named;

    u16 val;

    FontWeight()
        : FontWeight(NORMAL) {
    }

    FontWeight(_Named named)
        : val(static_cast<u16>(named)) {
    }

    FontWeight(u16 value)
        : val(value) {
    }

    void repr(Io::Emit &e) const {
        e("{}", val);
    }
};

using FontTag = Array<char, 4>;
static FontTag makeTag(Str tag) {
    if (tag.len() != 4)
        return {' ', ' ', ' ', ' '};
    return {tag[0], tag[1], tag[2], tag[3]};
}

struct FontFeature {
    enum struct _Value {
        OFF,
        ON,
    };

    FontTag tag;
    isize val;

    FontFeature(Str tag, isize val)
        : tag(makeTag(tag)), val(val) {
    }

    FontFeature(Str tag, _Value val)
        : tag(makeTag(tag)), val(static_cast<isize>(val)) {
    }

    void repr(Io::Emit &e) const {
        e("(font-feature {} {})", tag, val);
    }
};

struct FontVariation {
    FontTag tag;
    f64 val;

    FontVariation(Str tag, f64 val)
        : tag(makeTag(tag)), val(val) {
    }

    void repr(Io::Emit &e) const {
        e("(font-variation {} {})", tag, val);
    }
};

struct FontSource {
    Mime::Url url;
    Opt<String> format;

    void repr(Io::Emit &e) const {
        e("(font-source {}", url);
        if (format)
            e(" format({})", *format);
        e(")");
    }
};

// MARK: FontFace --------------------------------------------------------------

struct FontFace {
    String family;

    Vec<FontSource> sources;

    FontStyle style = FontStyle::NORMAL;
    FontWeight weight = FontWeight::NORMAL;
    FontWidth width = FontWidth::NORMAL;

    Vec<Range<Rune>> unicodeRange;

    Vec<FontFeature> features;
    Vec<FontVariation> variations;

    Opt<Percent> ascentOverride = NONE;
    Opt<Percent> descentOverride = NONE;
    Opt<Percent> lineGapOverride = NONE;
    Opt<Percent> sizeAdjust = NONE;

    FontDisplay display = FontDisplay::AUTO;

    bool valid() const {
        return any(family) and any(sources);
    }
};

// MARK: Props -----------------------------------------------------------------

// https://drafts.csswg.org/css-fonts

// MARK: ascent-override
// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc
struct AscentOverrideFontProp {
    Opt<Percent> value;

    static Str name() { return "ascent-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) const {
        f.ascentOverride = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = try$(parseValue<Percent>(c));
        }

        return Ok();
    }
};

// MARK: descent-override
// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc
struct DescentOverrideStyleProp {
    Opt<Percent> value;

    static Str name() { return "descent-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) const {
        f.descentOverride = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = try$(parseValue<Percent>(c));
        }

        return Ok();
    }
};

// MARK: font-display
// https://drafts.csswg.org/css-fonts/#font-display-desc
struct FontDisplayFontProp {
    FontDisplay value;

    static Str name() { return "font-display"; }

    static auto initial() { return FontDisplay::AUTO; }

    void apply(FontFace &f) const {
        f.display = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = FontDisplay::AUTO;
        } else if (c.skip(Css::Token::ident("block"))) {
            value = FontDisplay::BLOCK;
        } else if (c.skip(Css::Token::ident("swap"))) {
            value = FontDisplay::SWAP;
        } else if (c.skip(Css::Token::ident("fallback"))) {
            value = FontDisplay::FALLBACK;
        } else if (c.skip(Css::Token::ident("optional"))) {
            value = FontDisplay::OPTIONAL;
        } else {
            logWarn("expected font-display value");
            return Error::invalidData("expected font-display value");
        }

        return Ok();
    }
};

// MARK: font-family
// https://drafts.csswg.org/css-fonts/#font-family-desc
struct FontFamilyFontProp {
    String value;

    static Str name() { return "font-family"; }

    static auto initial() { return Vec<String>{"serif"s}; }

    void apply(FontFace &f) const {
        f.family = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        if (c.peek() == Css::Token::STRING) {
            value = try$(parseValue<String>(c));
        } else if (c.peek() == Css::Token::IDENT) {
            value = c.next().token.data;
        } else {
            return Error::invalidData("expected font-family value");
        }

        return Ok();
    }
};

// MARK: font-stretch
// https://drafts.csswg.org/css-fonts/#font-width-prop
struct FontWidthFontProp {
    FontWidth value;

    static Str name() { return "font-stretch"; }

    static auto initial() { return FontWidth::NORMAL; }

    void apply(FontFace &f) const {
        f.width = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        if (c.skip(Css::Token::ident("ultra-condensed"))) {
            value = FontWidth::ULTRA_CONDENSED;
        } else if (c.skip(Css::Token::ident("extra-condensed"))) {
            value = FontWidth::EXTRA_CONDENSED;
        } else if (c.skip(Css::Token::ident("condensed"))) {
            value = FontWidth::CONDENSED;
        } else if (c.skip(Css::Token::ident("semi-condensed"))) {
            value = FontWidth::SEMI_CONDENSED;
        } else if (c.skip(Css::Token::ident("normal"))) {
            value = FontWidth::NORMAL;
        } else if (c.skip(Css::Token::ident("semi-expanded"))) {
            value = FontWidth::SEMI_EXPANDED;
        } else if (c.skip(Css::Token::ident("expanded"))) {
            value = FontWidth::EXPANDED;
        } else if (c.skip(Css::Token::ident("extra-expanded"))) {
            value = FontWidth::EXTRA_EXPANDED;
        } else if (c.skip(Css::Token::ident("ultra-expanded"))) {
            value = FontWidth::ULTRA_EXPANDED;
        } else {
            value = try$(parseValue<Percent>(c));
        }

        return Ok();
    }
};

// MARK: font-style
// https://drafts.csswg.org/css-fonts/#font-style-desc
struct FontStyleFontProp {
    FontStyle value;

    static Str name() { return "font-style"; }

    static auto initial() { return FontStyle::NORMAL; }

    void apply(FontFace &f) const {
        f.style = value;
    }

    Res<> parse(Cursor<Css::Sst> &c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = FontStyle::NORMAL;
        } else if (c.skip(Css::Token::ident("italic"))) {
            value = FontStyle::ITALIC;
        } else if (c.skip(Css::Token::ident("oblique"))) {
            value = FontStyle::OBLIQUE;
        } else {
            value = FontStyle::NORMAL;
        }

        return Ok();
    }
};

// MARK: font-weight
// https://drafts.csswg.org/css-fonts/#font-weight-desc
struct FontWeightFontProp {
    FontWeight value;

    static Str name() { return "font-weight"; }

    static auto initial() { return FontWeight::NORMAL; }

    void apply(FontFace &f) const {
        f.weight = value;
    }
};

// MARK: font-feature-settings
// https://drafts.csswg.org/css-fonts/#font-feature-settings-prop
struct FontFeatureSettingsFontProp {
    Vec<FontFeature> value;

    static Str name() { return "font-feature-settings"; }

    static Vec<FontFeature> initial() { return {}; }

    void apply(FontFace &f) const {
        f.features = value;
    }
};

// MARK: font-variation-settings
// https://drafts.csswg.org/css-fonts/#font-variation-settings-prop
struct FontVariationSettingsFontProp {
    Vec<FontVariation> value;

    static Str name() { return "font-variation-settings"; }

    static Vec<FontVariation> initial() { return {}; }

    void apply(FontFace &f) const {
        f.variations = value;
    }
};

// MARK: line-gap-override
// https://drafts.csswg.org/css-fonts/#font-metrics-override-desc
struct LineGapOverrideFontProp {
    Opt<Percent> value;

    static Str name() { return "line-gap-override"; }

    static auto initial() { return NONE; }

    void apply(FontFace &f) const {
        f.lineGapOverride = value;
    }
};

// MARK: size-adjust
// https://drafts.csswg.org/css-fonts-5/#size-adjust-desc
struct SizeAdjustFontProp {
    Opt<Percent> value;

    static Str name() { return "size-adjust"; }

    static auto initial() {
        return NONE;
    }

    void apply(FontFace &f) const {
        f.sizeAdjust = value;
    }
};

// MARK: src
// https://drafts.csswg.org/css-fonts/#src-desc
struct SrcFontProp {
    Vec<FontSource> value;

    static Str name() { return "src"; }

    static auto initial() {
        return Vec<FontSource>{};
    }

    void apply(FontFace &f) const {
        f.sources = value;
    }
};

// MARK: unicode-range
// https://drafts.csswg.org/css-fonts/#unicode-range-desc
struct UnicodeRangeFontProp {
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
    AscentOverrideFontProp,
    DescentOverrideStyleProp,
    FontDisplayFontProp,
    FontFamilyFontProp,
    FontWidthFontProp,
    FontStyleFontProp,
    FontWeightFontProp,
    FontFeatureSettingsFontProp,
    FontVariationSettingsFontProp,
    LineGapOverrideFontProp,
    SizeAdjustFontProp,
    SrcFontProp,
    UnicodeRangeFontProp

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
