#pragma once

#include <karm-mime/url.h>
#include <vaev-base/font.h>
#include <vaev-css/parser.h>

#include "values.h"

namespace Vaev::Style {

// MARK: FontFace --------------------------------------------------------------

struct FontFace {
    String family;

    Vec<FontSource> sources;

    Union<None, FontStyle, Range<Angle>> style = FontStyle{FontStyle::NORMAL};

    Opt<Range<FontWeight>> weight = FontWeight::NORMAL;
    Opt<Range<FontWidth>> width = FontWidth::NORMAL;

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

// https://www.w3.org/TR/css-fonts-4

// MARK: font-family
// https://www.w3.org/TR/css-fonts-4/#font-family-desc
struct FontFamilyDesc {
    String value = initial();

    static Str name() { return "font-family"; }

    static String initial() { return "serif"s; }

    void apply(FontFace& f) const {
        f.family = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
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

// MARK: src
// https://www.w3.org/TR/css-fonts-4/#src-desc
struct SrcDesc {
    Vec<FontSource> value;

    static Str name() { return "src"; }

    static auto initial() {
        return Vec<FontSource>{};
    }

    void apply(FontFace& f) const {
        f.sources = value;
    }
};

// MARK: font-style
// https://www.w3.org/TR/css-fonts-4/#font-style-desc
struct FontStyleDesc {
    Union<None, FontStyle, Range<Angle>> value;

    static Str name() { return "font-style"; }

    static auto initial() { return FontStyle::NORMAL; }

    void apply(FontFace& f) const {
        f.style = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
            return Ok();
        }

        auto style = try$(parseValue<FontStyle>(c));
        if (style != FontStyle::OBLIQUE) {
            value = style;
            return Ok();
        }

        auto val = parseValue<Angle>(c);
        if (not val) {
            value = style;
            return Ok();
        }

        value = Range<Angle>::fromStartEnd(style.obliqueAngle, val.unwrap());
        return Ok();
    }
};

// MARK: font-weight
// https://www.w3.org/TR/css-fonts-4/#font-weight-desc
struct FontWeightDesc {
    Opt<Range<FontWeight>> value;

    static Str name() { return "font-weight"; }

    static auto initial() { return FontWeight::NORMAL; }

    void apply(FontFace& f) const {
        f.weight = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
            return Ok();
        }

        auto weight = try$(parseValue<FontWeight>(c));

        auto val = parseValue<FontWeight>(c);
        if (not val) {
            value = weight;
            return Ok();
        }

        value = Range<FontWeight>::fromStartEnd(weight, val.unwrap());

        return Ok();
    }
};

// MARK: font-width
// https://www.w3.org/TR/css-fonts-4/#font-width-prop
struct FontWidthDesc {
    Opt<Range<FontWidth>> value = initial();

    static Str name() { return "font-width"; }

    static Opt<Range<FontWidth>> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.width = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
            return Ok();
        }

        auto width = try$(parseValue<FontWidth>(c));

        auto val = parseValue<FontWidth>(c);
        if (not val) {
            value = width;
            return Ok();
        }

        value = Range<FontWidth>::fromStartEnd(width, val.unwrap());

        return Ok();
    }
};

// MARK: unicode-range
// https://www.w3.org/TR/css-fonts-4/#unicode-range-desc
struct UnicodeRangeDesc {
    Vec<Range<Rune>> value;

    static Str name() { return "unicode-range"; }

    static auto initial() {
        return Vec<Range<Rune>>{};
    }

    void apply(FontFace& f) const {
        f.unicodeRange = value;
    }
};

// MARK: font-feature-settings
// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
struct FontFeatureSettingsDesc {
    Vec<FontFeature> value;

    static Str name() { return "font-feature-settings"; }

    static Vec<FontFeature> initial() { return {}; }

    void apply(FontFace& f) const {
        f.features = value;
    }
};

// MARK: font-variation-settings
// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
struct FontVariationSettingsDesc {
    Vec<FontVariation> value;

    static Str name() { return "font-variation-settings"; }

    static Vec<FontVariation> initial() { return {}; }

    void apply(FontFace& f) const {
        f.variations = value;
    }
};

// MARK: font-named-instance
// https://www.w3.org/TR/css-fonts-4/#font-named-instance
struct FontNamedInstanceDesc {
    Opt<String> value = initial();

    static Str name() { return "font-named-instance"; }

    static Opt<String> initial() { return NONE; }

    void apply(FontFace&) const {
        // TODO
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
        } else {
            value = try$(parseValue<String>(c));
        }

        return Ok();
    }
};

// MARK: font-display
// https://www.w3.org/TR/css-fonts-4/#font-display-desc
struct FontDisplayDesc {
    FontDisplay value = initial();

    static Str name() { return "font-display"; }

    static FontDisplay initial() { return FontDisplay::AUTO; }

    void apply(FontFace& f) const {
        f.display = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
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

// MARK: ascent-override
// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
struct AscentOverrideDesc {
    Opt<Percent> value = initial();

    static Str name() { return "ascent-override"; }

    // NOTE: normal is NONE
    static Opt<Percent> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.ascentOverride = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = try$(parseValue<Percent>(c));
        }

        return Ok();
    }
};

// MARK: descent-override
// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
struct DescentOverrideStyleProp {
    Opt<Percent> value = initial();

    static Str name() { return "descent-override"; }

    // NOTE: normal is NONE
    static Opt<Percent> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.descentOverride = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = try$(parseValue<Percent>(c));
        }

        return Ok();
    }
};

// MARK: line-gap-override
// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
struct LineGapOverrideDesc {
    Opt<Percent> value = initial();

    static Str name() { return "line-gap-override"; }

    static Opt<Percent> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.lineGapOverride = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = try$(parseValue<Percent>(c));
        }

        return Ok();
    }
};

// MARK: size-adjust
// https://www.w3.org/TR/css-fonts-5/#size-adjust-desc
struct SizeAdjustDesc {
    Percent value = initial();

    static Str name() { return "size-adjust"; }

    static Percent initial() { return Percent{100}; }

    void apply(FontFace& f) const {
        f.sizeAdjust = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Percent>(c));
        return Ok();
    }
};

using _FontDesc = Union<
    FontFamilyDesc,
    SrcDesc,
    FontStyleDesc,
    FontWeightDesc,
    FontWidthDesc,

    UnicodeRangeDesc,

    FontFeatureSettingsDesc,
    FontVariationSettingsDesc,
    FontNamedInstanceDesc,

    FontDisplayDesc,

    AscentOverrideDesc,
    DescentOverrideStyleProp,
    LineGapOverrideDesc,
    SizeAdjustDesc

    /**/
    >;

struct FontDesc : public _FontDesc {
    using _FontDesc::_FontDesc;

    static constexpr Array LEGACY_ALIAS = {
        Pair<Str>("font-stretch", "font-width"),
    };

    static Str mapLegacyAlias(Str name) {
        for (auto const& [from, to] : LEGACY_ALIAS) {
            if (from == name) [[unlikely]]
                return to;
        }

        return name;
    }

    Str name() const {
        return visit([](auto const& p) {
            return p.name();
        });
    }

    void apply(FontFace& f) const {
        visit([&](auto const& p) {
            p.apply(f);
        });
    }

    void repr(Io::Emit& e) const {
        e("({}", name());
        visit([&](auto const& p) {
            e(" {}", p.value);
        });
        e(")");
    }
};

} // namespace Vaev::Style
