#pragma once

#include <karm-base/range.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/emit.h>
#include <karm-mime/url.h>
#include <vaev-base/angle.h>
#include <vaev-base/percent.h>

namespace Vaev::Style {

enum struct FontDisplay {
    AUTO,
    BLOCK,
    SWAP,
    FALLBACK,
    OPTIONAL,

    _LEN,
};

struct FontStretch {
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

    FontStretch()
        : FontStretch(NORMAL) {
    }

    FontStretch(_Named named)
        : _val(toPercent(named)) {
    }

    FontStretch(Percent value)
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

struct FontFace {
    String family;
    Opt<Percent> ascentOverride = NONE;
    Opt<Percent> descentOverride = NONE;
    FontDisplay display = FontDisplay::AUTO;
    FontStretch stretch = FontStretch::NORMAL;
    FontStyle style = FontStyle::NORMAL;
    FontWeight weight = FontWeight::NORMAL;
    Vec<FontFeature> features;
    Vec<FontVariation> variations;
    Opt<Percent> lineGapOverride = NONE;
    Opt<Percent> sizeAdjust = NONE;
    Vec<FontSource> sources;
    Vec<Range<Rune>> unicodeRange;

    bool valid() const {
        return any(family) and any(sources);
    }
};

} // namespace Vaev::Style
