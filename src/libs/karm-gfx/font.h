#pragma once

#include <karm-logger/logger.h>
#include <karm-math/rect.h>

import Karm.Core;

namespace Karm::Gfx {

struct Canvas;

struct Glyph {
    u16 index;
    u16 font;

    static Glyph const TOFU;

    bool operator==(Glyph const& other) const = default;

    auto operator<=>(Glyph const& other) const = default;
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

struct FontWeight : Distinct<u16, struct _FontWeightTag> {
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

    void repr(Io::Emit& e) const {
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

    FontWeight lighter() const {
        if (value() < 100)
            return FontWeight{value()};
        if (value() < 350)
            return FontWeight::THIN;
        if (value() < 550)
            return FontWeight::THIN;
        if (value() < 750)
            return FontWeight::REGULAR;
        if (value() < 900)
            return FontWeight::BOLD;
        return FontWeight::BOLD;
    }

    FontWeight bolder() const {
        if (value() < 100)
            return FontWeight::REGULAR;
        if (value() < 350)
            return FontWeight::REGULAR;
        if (value() < 550)
            return FontWeight::BOLD;
        if (value() < 750)
            return FontWeight::BLACK;
        if (value() < 900)
            return FontWeight::BLACK;
        return FontWeight{value()};
    }

    FontWeight operator+(FontWeight const& other) const {
        return FontWeight(value() + other.value());
    }

    FontWeight operator-(FontWeight const& other) const {
        return FontWeight(value() - other.value());
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

struct FontStretch : Distinct<u16, struct _FontStretchTag> {
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

    void repr(Io::Emit& e) const {
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

// MARK: FontAttrs -------------------------------------------------------------

struct FontAttrs {
    Symbol family = ""_sym;
    FontWeight weight = FontWeight::REGULAR;
    FontStretch stretch = FontStretch::NORMAL;
    FontStyle style = FontStyle::NORMAL;
    Monospace monospace = Monospace::NO;

    void repr(Io::Emit& e) const {
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

    auto operator<=>(FontAttrs const& other) const {
        // NOTE: Comparison order is important
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

// MARK: Font Metrics ----------------------------------------------------------

/**
   _____       _             _                          <- ascend
  / ____|     | |           | |                         <- captop/cap height
 | |  __ _   _| |_ ___ _ __ | |__   ___ _ __ __ _
 | | |_ | | | | __/ _ \ '_ \| '_ \ / _ \ '__/ _` |
 | |__| | |_| | ||  __/ | | | |_) |  __/ | | (_| |
  \_____|\__,_|\__\___|_| |_|_.__/ \___|_|  \__, |      <- baseline (origin)
                                             __/ |
                                            |___/       <- descend
 | ---- |                                               ...advance

                                                        <- line gap
*/
struct FontMetrics {
    f64 ascend;
    f64 captop;
    f64 descend;
    f64 linegap;
    f64 advance;
    f64 xHeight;

    f64 lineheight() {
        return ascend + descend + linegap;
    }

    // MARK: Baselines ---------------------------------------------------------
    // Theses are relative to the origin (0, 0) of the font metrics.

    f64 xMiddleBaseline() {
        return (ascend + descend) / 2;
    }

    f64 alphabeticBaseline() {
        return ascend;
    }

    // MARK: Transformations ---------------------------------------------------

    FontMetrics combine(FontMetrics other) {
        return {
            .ascend = Karm::max(ascend, other.ascend),
            .captop = Karm::max(captop, other.captop),
            .descend = Karm::max(descend, other.descend),
            .linegap = Karm::max(linegap, other.linegap),
            .advance = Karm::max(advance, other.advance),
            .xHeight = Karm::max(xHeight, other.xHeight),
        };
    }

    FontMetrics scale(f64 factor) {
        return {
            .ascend = ascend * factor,
            .captop = captop * factor,
            .descend = descend * factor,
            .linegap = linegap * factor,
            .advance = advance * factor,
            .xHeight = xHeight * factor,
        };
    }
};

struct FontMeasure {
    Math::Rectf capbound;
    Math::Rectf linebound;
    Math::Vec2f baseline;
};

struct Fontface {
    static Rc<Fontface> fallback();

    virtual ~Fontface() = default;

    virtual FontMetrics metrics() = 0;

    virtual FontAttrs attrs() const = 0;

    virtual Glyph glyph(Rune rune) = 0;

    virtual f64 advance(Glyph glyph) = 0;

    virtual f64 kern(Glyph prev, Glyph curr) = 0;

    virtual void contour(Canvas& g, Glyph glyph) const = 0;
};

struct FontAdjust {
    f64 ascent = 1;
    f64 descent = 1;
    f64 linegap = 1;
    f64 sizeAdjust = 1;

    FontAdjust combine(FontAdjust other) const {
        return {
            .ascent = ascent * other.ascent,
            .descent = descent * other.descent,
            .linegap = linegap * other.linegap,
            .sizeAdjust = sizeAdjust * other.sizeAdjust,
        };
    }
};

struct FontFamily : Fontface {
    struct Member {
        FontAdjust adjust;
        Rc<Fontface> face;
        Opt<Ranges<Range<Rune>>> ranges;
    };

    FontAdjust _adjust;
    Vec<Member> _members;

    FontFamily(Vec<Member> members) : _members(std::move(members)) {}

    FontMetrics metrics() override {
        FontMetrics metrics = {};

        for (auto& member : _members) {
            auto m = metrics.combine(member.face->metrics());
            auto& a = member.adjust;

            m.ascend *= a.ascent * _adjust.ascent;
            m.descend *= a.descent * _adjust.descent;
            m.linegap *= a.linegap * _adjust.linegap;

            metrics = metrics.combine(m);
        }

        return metrics;
    }

    FontAttrs attrs() const override {
        FontAttrs attrs;

        StringBuilder familyName;
        for (auto& member : _members) {
            if (familyName.len())
                familyName.append(" | "s);
            familyName.append(member.face->attrs().family.str());
        }

        attrs.family = Symbol::from(familyName.take());

        return attrs;
    }

    Glyph glyph(Rune rune) override {
        Glyph res = Glyph::TOFU;

        for (usize i = 0; i < _members.len(); i++) {
            auto& member = _members[i];
            if (member.ranges and not member.ranges->contains({rune, 1})) {
                continue;
            }

            res = member.face->glyph(rune);
            if (res != Glyph::TOFU) {
                res.font = i;
                break;
            }
        }

        if (res == Glyph::TOFU)
            logWarn("failed to find glyph for rune: {:x}", rune);

        return res;
    }

    f64 advance(Glyph glyph) override {
        auto& member = _members[glyph.font];
        auto a = member.face->advance(glyph);
        return a * member.adjust.sizeAdjust * _adjust.sizeAdjust;
    }

    f64 kern(Glyph prev, Glyph curr) override {
        if (prev.font != curr.font)
            return 0;

        auto& member = _members[prev.font];
        auto k = member.face->kern(prev, curr);
        return k * member.adjust.sizeAdjust * _adjust.sizeAdjust;
    }

    void contour(Canvas& g, Glyph glyph) const override;
};

struct Font {
    Rc<Fontface> fontface;
    f64 fontsize;
    f64 lineheight = 1.2;

    static Font fallback();

    FontMetrics metrics();

    Glyph glyph(Rune rune);

    f64 advance(Glyph glyph);

    f64 kern(Glyph prev, Glyph curr);

    FontMeasure measure(Glyph glyph);

    void contour(Canvas& g, Glyph glyph);

    f64 fontSize();

    f64 xHeight();

    f64 capHeight();

    f64 zeroAdvance();

    f64 lineHeight();
};

} // namespace Karm::Gfx
