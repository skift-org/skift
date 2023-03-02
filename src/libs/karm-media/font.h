#pragma once

#include <karm-base/rc.h>
#include <karm-math/rect.h>

namespace Karm::Gfx {

struct Context;

}; // namespace Karm::Gfx

namespace Karm::Media {

enum FontWeight {
    THIN = 100,
    EXTRA_LIGHT = 200,
    LIGHT = 300,
    REGULAR = 400,
    MEDIUM = 500,
    SEMI_BOLD = 600,
    BOLD = 700,
    EXTRA_BOLD = 800,
    BLACK = 900,
};

enum FontWidth {
    ULTRA_CONDENSED = 100,
    EXTRA_CONDENSED = 200,
    CONDENSED = 300,
    SEMI_CONDENSED = 400,
    NORMAL = 500,
    SEMI_EXPANDED = 600,
    EXPANDED = 700,
    EXTRA_EXPANDED = 800,
    ULTRA_EXPANDED = 900
};

enum FontSlant {
    UPRIGHT = 0,
    ITALIC = 1,
    OBLIQUE = 2,
};

struct FontAttrs {
    FontWeight weight = REGULAR;
    FontWidth width = NORMAL;
    FontSlant slant = UPRIGHT;
};

/**
   _____       _             _                          <- ascend
  / ____|     | |           | |                         <- captop
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
};

struct FontMesure {
    Math::Rectf capbound;
    Math::Rectf linebound;
    Math::Vec2f baseline;
};

struct Fontface {
    static Strong<Fontface> fallback();

    virtual ~Fontface() = default;

    virtual FontMetrics metrics() const = 0;

    virtual f64 advance(Rune c) const = 0;

    virtual void contour(Gfx::Context &g, Rune rune) const = 0;

    virtual f64 units() const = 0;
};

struct Font {
    Strong<Fontface> fontface;
    f64 fontsize;
    f64 lineheight = 1.2;

    static Font fallback();

    FontMetrics metrics() const;

    f64 advance(Rune c) const;

    FontMesure mesureRune(Rune r) const;

    FontMesure mesureStr(Str str) const;
};

} // namespace Karm::Media
