#pragma once

#include <karm-base/ranges.h>
#include <karm-base/rc.h>
#include <karm-gfx/context.h>
#include <karm-math/rect.h>

#include "base.h"

namespace Karm::Text {

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

    f64 lineheight() {
        return ascend + descend + linegap;
    }

    FontMetrics combine(FontMetrics other) {
        return {
            .ascend = ::max(ascend, other.ascend),
            .captop = ::max(captop, other.captop),
            .descend = ::max(descend, other.descend),
            .linegap = ::max(linegap, other.linegap),
            .advance = ::max(advance, other.advance),
        };
    }
};

struct FontMeasure {
    Math::Rectf capbound;
    Math::Rectf linebound;
    Math::Vec2f baseline;
};

struct Fontface {
    static Strong<Fontface> fallback();

    virtual ~Fontface() = default;

    virtual FontMetrics metrics() const = 0;

    virtual FontAttrs attrs() const = 0;

    virtual Glyph glyph(Rune rune) = 0;

    virtual f64 advance(Glyph glyph) = 0;

    virtual f64 kern(Glyph prev, Glyph curr) = 0;

    virtual void contour(Gfx::Context &g, Glyph glyph) const = 0;
};



struct Font {
    Strong<Fontface> fontface;
    f64 fontsize;
    f64 lineheight = 1.2;

    static Font fallback();

    FontMetrics metrics() const;

    Glyph glyph(Rune rune);

    f64 advance(Glyph glyph);

    f64 kern(Glyph prev, Glyph curr);

    FontMeasure measure(Glyph glyph);

    void contour(Gfx::Context &g, Glyph glyph) {
        g.scale(fontsize);
        fontface->contour(g, glyph);
    }
};

} // namespace Karm::Text
