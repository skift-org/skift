#pragma once

#include <karm-base/checked.h>
#include <karm-base/distinct.h>
#include <karm-base/string.h>
#include <karm-io/emit.h>

#include "_fwd.h"

namespace Karm::Text {

struct Glyph {
    u16 index;
    u16 font;

    static Glyph const TOFU;

    bool operator==(Glyph const &other) const = default;

    auto operator<=>(Glyph const &other) const = default;
};

constexpr Glyph Glyph::TOFU{0, 0};

// MARK: FontStyle -------------------------------------------------------------

enum struct FontStyle {
    NORMAL,
    OBLIQUE,
    ITALIC,

    NO_MATCH,
    _LEN,
};

// MARK: Fonteight -------------------------------------------------------------

struct FontWeight : public Distinct<u16, struct _FontWeightTag> {
    using Distinct::Distinct;

    static FontWeight const THIN;
    static FontWeight const EXTRA_LIGHT;
    static FontWeight const LIGHT;
    static FontWeight const REGULAR;
    static FontWeight const MEDIUM;
    static FontWeight const SEMI_BOLD;
    static FontWeight const BOLD;
    static FontWeight const EXTRA_BOLD;
    static FontWeight const BLACK;

    static FontWeight const NO_MATCH;

    void repr(Io::Emit &e) const {
        if (value() <= 100)
            e("THIN");
        else if (value() <= 200)
            e("EXTRA_LIGHT");
        else if (value() <= 300)
            e("LIGHT");
        else if (value() <= 400)
            e("REGULAR");
        else if (value() <= 500)
            e("MEDIUM");
        else if (value() <= 600)
            e("SEMI_BOLD");
        else if (value() <= 700)
            e("BOLD");
        else if (value() <= 800)
            e("EXTRA_BOLD");
        else if (value() <= 900)
            e("BLACK");
        else if (value() == NO_MATCH.value())
            e("NO_MATCH");
        else
            e("UNKNOWN");
    }

    u16 delta(FontWeight other) {
        if (other > *this)
            return (other - *this).value();
        return (*this - other).value();
    }
};

constexpr FontWeight FontWeight::THIN{100};
constexpr FontWeight FontWeight::EXTRA_LIGHT{200};
constexpr FontWeight FontWeight::LIGHT{300};
constexpr FontWeight FontWeight::REGULAR{400};
constexpr FontWeight FontWeight::MEDIUM{500};
constexpr FontWeight FontWeight::SEMI_BOLD{600};
constexpr FontWeight FontWeight::BOLD{700};
constexpr FontWeight FontWeight::EXTRA_BOLD{800};
constexpr FontWeight FontWeight::BLACK{900};

constexpr FontWeight FontWeight::NO_MATCH{Limits<u16>::MAX};

// MARK: FontStretch -----------------------------------------------------------

struct FontStretch : public Distinct<u16, struct _FontStretchTag> {
    using Distinct::Distinct;

    static FontStretch const ULTRA_CONDENSED;
    static FontStretch const EXTRA_CONDENSED;
    static FontStretch const CONDENSED;
    static FontStretch const SEMI_CONDENSED;
    static FontStretch const NORMAL;
    static FontStretch const SEMI_EXPANDED;
    static FontStretch const EXPANDED;
    static FontStretch const EXTRA_EXPANDED;
    static FontStretch const ULTRA_EXPANDED;

    static FontStretch const NO_MATCH;

    void repr(Io::Emit &e) const {
        if (value() <= 100)
            e("ULTRA_CONDENSED");
        else if (value() <= 200)
            e("EXTRA_CONDENSED");
        else if (value() <= 300)
            e("CONDENSED");
        else if (value() <= 400)
            e("SEMI_CONDENSED");
        else if (value() <= 500)
            e("NORMAL");
        else if (value() <= 600)
            e("SEMI_EXPANDED");
        else if (value() <= 700)
            e("EXPANDED");
        else if (value() <= 800)
            e("EXTRA_EXPANDED");
        else if (value() <= 900)
            e("ULTRA_EXPANDED");
        else if (value() == NO_MATCH.value())
            e("NO_MATCH");
        else
            e("UNKNOWN");
    }

    u16 delta(FontStretch other) {
        if (other > *this) {
            return (other - *this).value();
        } else {
            return (*this - other).value();
        }
    }
};

constexpr FontStretch FontStretch::ULTRA_CONDENSED{100};
constexpr FontStretch FontStretch::EXTRA_CONDENSED{200};
constexpr FontStretch FontStretch::CONDENSED{300};
constexpr FontStretch FontStretch::SEMI_CONDENSED{400};
constexpr FontStretch FontStretch::NORMAL{500};
constexpr FontStretch FontStretch::SEMI_EXPANDED{600};
constexpr FontStretch FontStretch::EXPANDED{700};
constexpr FontStretch FontStretch::EXTRA_EXPANDED{800};
constexpr FontStretch FontStretch::ULTRA_EXPANDED{900};

constexpr FontStretch FontStretch::NO_MATCH{Limits<u16>::MAX};

// MARK: Monospace -------------------------------------------------------------

enum struct Monospace {
    NO,
    YES,

    _LEN,
};

// MARK: Family ----------------------------------------------------------------

enum struct GenericFamily {
    SERIF,
    SANS_SERIF,
    MONOSPACE,
    CURSIVE,
    FANTASY,

    SYSTEM,
    EMOJI,
    MATH,
    FANGSONG,

    _LEN,
};

using Family = Union<GenericFamily, Str>;

// MARK: FontAttrs -------------------------------------------------------------

struct FontAttrs {
    String family;
    FontWeight weight = FontWeight::REGULAR;
    FontStretch stretch = FontStretch::NORMAL;
    FontStyle style = FontStyle::NORMAL;
    Monospace monospace = Monospace::NO;

    void repr(Io::Emit &e) const {
        e.ln("family: {#}", family);
        e.ln("weight: {}", weight);
        e.ln("stretch: {}", stretch);
        e.ln("style: {}", style);
        e.ln("monospace: {}", monospace);
    }

    bool normal() const {
        return weight == FontWeight::REGULAR and
               stretch == FontStretch::NORMAL and
               style == FontStyle::NORMAL and
               monospace == Monospace::NO;
    }

    auto operator<=>(FontAttrs const &other) const {
        // NOTE: Comparaison order is important

        auto ordr = family <=> other.family;
        if (ordr != 0)
            return ordr;

        ordr = style <=> other.style;
        if (ordr != 0)
            return ordr;

        ordr = stretch <=> other.stretch;
        if (ordr != 0)
            return ordr;

        return weight <=> other.weight;
    }
};

} // namespace Karm::Text
