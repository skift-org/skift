#pragma once

#include <karm-base/rc.h>
#include <karm-math/rect.h>

namespace Karm::Gfx {
struct Context;
};

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

struct FontAttrs {
    int size;
    FontWeight weight;
    bool italic;
    bool monospace;
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
    double ascend;
    double captop;
    double descend;
    double linegap;
    double advance;
};

struct FontMesure {
    Math::Rectf capbound;
    Math::Rectf linebound;
    Math::Vec2f baseline;
};

struct Font {
    static Strong<Font> fallback();

    virtual ~Font() = default;

    virtual FontMetrics metrics() const = 0;
    virtual double advance(Rune c) const = 0;
    virtual void fillRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const = 0;
    virtual void strokeRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const {
        fillRune(g, baseline, rune);
    }

    FontMesure mesureRune(Rune r) const {
        auto adv = advance(r);
        auto m = metrics();

        return {
            .capbound = {adv, m.captop + m.descend},
            .linebound = {adv, m.ascend + m.descend},
            .baseline = {0, m.ascend},
        };
    }

    FontMesure mesureStr(Str str) const {
        double adv = 0;
        for (auto r : iterRunes(str)) {
            adv += advance(r);
        }

        auto m = metrics();
        return {
            .capbound = {adv, m.captop + m.descend},
            .linebound = {adv, m.ascend + m.descend},
            .baseline = {0, m.ascend},
        };
    }

    void fillStr(Gfx::Context &g, Math::Vec2i baseline, Str str) const {
        for (auto r : iterRunes(str)) {
            fillRune(g, baseline, r);
            baseline.x += advance(r);
        }
    }

    void strokeStr(Gfx::Context &g, Math::Vec2i baseline, Str str) const {
        for (auto r : iterRunes(str)) {
            strokeRune(g, baseline, r);
            baseline.x += advance(r);
        }
    }
};

} // namespace Karm::Media
