#pragma once

#include <karm-base/range.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-mime/url.h>
#include <web-base/angle.h>
#include <web-base/percent.h>

namespace Web::Style {

enum struct FontDisplay {
    AUTO,
    BLOCK,
    SWAP,
    FALLBACK,
    OPTIONAL,
};

struct FontStretch {
    enum struct _Named {
        ULTRA_CONDENSED,
        EXTRA_CONDENSED,
        CONDENSED,
        SEMI_CONDENSED,
        NORMAL,
        SEMI_EXPANDED,
        EXPANDED,
        EXTRA_EXPANDED,
        ULTRA_EXPANDED,
    };

    static Percent toPercent(_Named stretch) {
        switch (stretch) {
        case _Named::ULTRA_CONDENSED:
            return Percent{50};

        case _Named::EXTRA_CONDENSED:
            return Percent{62.5};

        case _Named::CONDENSED:
            return Percent{75};

        case _Named::SEMI_CONDENSED:
            return Percent{87.5};

        case _Named::NORMAL:
            return Percent{100};

        case _Named::SEMI_EXPANDED:
            return Percent{112.5};

        case _Named::EXPANDED:
            return Percent{125};

        case _Named::EXTRA_EXPANDED:
            return Percent{150};

        case _Named::ULTRA_EXPANDED:
            return Percent{200};
        }
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
};

struct FontStyle {
    enum struct _Named {
        NORMAL,
        ITALIC,
        OBLIQUE,
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
};

struct FontVariation {
    FontTag tag;
    f64 val;

    FontVariation(Str tag, f64 val)
        : tag(makeTag(tag)), val(val) {
    }
};

struct FontSource {
    Mime::Url url;
    Opt<String> format;
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

} // namespace Web::Style
