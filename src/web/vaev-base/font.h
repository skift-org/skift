#pragma once

#include <karm-mime/url.h>
#include <karm-text/base.h>

#include "angle.h"
#include "length.h"
#include "percent.h"

namespace Vaev {

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

    constexpr FontWidth()
        : FontWidth(NORMAL) {
    }

    constexpr FontWidth(_Named named)
        : _val(toPercent(named)) {
    }

    constexpr FontWidth(Percent value)
        : _val(value) {
    }

    constexpr auto val() const {
        return _val;
    }

    void repr(Io::Emit& e) const {
        e("{}", _val);
    }

    bool operator==(FontWidth const& other) const {
        return _val == other._val;
    }

    auto operator<=>(FontWidth const& other) const {
        return _val <=> other._val;
    }

    Percent operator-(FontWidth const& other) const {
        return _val - other._val;
    }

    FontWidth operator+(Percent value) const {
        return FontWidth{_val + value};
    }
};

struct FontStyle {
    using enum Text::FontStyle;
    Text::FontStyle val;
    Angle obliqueAngle;

    constexpr FontStyle(Text::FontStyle named = NORMAL)
        : val(named) {
    }

    constexpr FontStyle(Angle angle)
        : val(OBLIQUE), obliqueAngle(angle) {}

    void repr(Io::Emit& e) const {
        e("{}", val);
        if (val == OBLIQUE)
            e(" {}", obliqueAngle);
    }

    bool operator==(FontStyle const& other) const {
        return val == other.val and
               (val != OBLIQUE or obliqueAngle == other.obliqueAngle);
    }
};

enum struct RelativeFontWeight : u8 {
    LIGHTER,
    BOLDER,
};

using _FontWeight = Union<Text::FontWeight, RelativeFontWeight>;

struct FontWeight : _FontWeight {
    using _FontWeight::_FontWeight;

    bool isRelative() const {
        return is<RelativeFontWeight>();
    }

    FontWeight()
        : FontWeight(Text::FontWeight::REGULAR) {
    }

    void repr(Io::Emit& e) const {
        e("{}", static_cast<Union<Text::FontWeight, RelativeFontWeight>>(*this));
    }

    Text::FontWeight resolve() const {
        if (is<RelativeFontWeight>())
            return Text::FontWeight::REGULAR;

        return unwrap<Text::FontWeight>();
    }

    Text::FontWeight resolve(Text::FontWeight const& parent) const {
        if (is<RelativeFontWeight>()) {
            if (unwrap<RelativeFontWeight>() == RelativeFontWeight::LIGHTER)
                return parent.lighter();
            return parent.bolder();
        }

        return unwrap<Text::FontWeight>();
    }
};

struct FontSize {
    enum struct _Named {
        // absolute
        XX_SMALL,
        X_SMALL,
        SMALL,
        MEDIUM,
        LARGE,
        X_LARGE,
        XX_LARGE,

        // relative
        LARGER,
        SMALLER,

        // length/percent
        LENGTH,

        _LEN,
    };

    using enum _Named;

    _Named _named;
    PercentOr<Length> _value;

    constexpr FontSize(_Named named = MEDIUM)
        : _named(named), _value(Length{}) {
    }

    constexpr FontSize(PercentOr<Length> size)
        : _named(LENGTH), _value(size) {
    }

    _Named named() const {
        return _named;
    }

    PercentOr<Length> value() const {
        if (_named != LENGTH)
            panic("not a length");
        return _value;
    }

    void repr(Io::Emit& e) const {
        if (_named == LENGTH) {
            e("{}", _value);
        } else {
            e("{}", _named);
        }
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

    void repr(Io::Emit& e) const {
        e("(font-feature {} {})", tag, val);
    }

    bool operator==(FontFeature const& other) const {
        return tag == other.tag and val == other.val;
    }

    bool operator==(FontTag const& other) const {
        return tag == other;
    }

    bool operator==(_Value const& other) const {
        return val == static_cast<isize>(other);
    }
};

struct FontVariation {
    FontTag tag;
    f64 val;

    FontVariation(Str tag, f64 val)
        : tag(makeTag(tag)), val(val) {
    }

    void repr(Io::Emit& e) const {
        e("(font-variation {} {})", tag, val);
    }

    bool operator==(FontTag const& other) const {
        return tag == other;
    }
};

struct FontSource {
    Union<Mime::Url, Text::Family> identifier;
    Opt<String> format;

    FontSource(Mime::Url url) : identifier(url) {}

    FontSource(Text::Family family) : identifier(family) {}

    void repr(Io::Emit& e) const {
        e("(font-source {}", identifier);
        if (format)
            e(" format({})", *format);
        e(")");
    }
};

struct FontProps {
    Vec<Text::Family> families = {Text::GenericFamily::SANS_SERIF};
    Text::FontWeight weight = Text::FontWeight::REGULAR;
    FontWidth width = FontWidth::NORMAL;
    FontStyle style = FontStyle::NORMAL;
    FontSize size = FontSize::MEDIUM;

    void repr(Io::Emit& e) const {
        e("(font");
        e(" families={}", families);
        e(" weight={}", weight);
        e(" width={}", width);
        e(" style={}", style);
        e(" size={}", size);
        e(")");
    }
};

} // namespace Vaev
