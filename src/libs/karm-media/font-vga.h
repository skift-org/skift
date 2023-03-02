#pragma once

#include <karm-base/array.h>
#include <karm-gfx/context.h>

#include "font.h"

namespace Karm::Media {

struct VgaFontface : public Fontface {
    static constexpr isize WIDTH = 8;
    static constexpr isize HEIGHT = 8;

    static constexpr Array<u8, 1024> const DATA = {
#include "font-vga.inc"
    };

    FontMetrics metrics() const override {
        return {
            .ascend = 12,
            .captop = 10,
            .descend = 4,
            .linegap = 4,
            .advance = 8,
        };
    }

    f64 advance(Rune) const override {
        return 8;
    }

    void contour(Gfx::Context &g, Rune rune) const override {
        One<Ibm437> one;
        encodeOne<Ibm437>(rune, one);

        for (isize y = 0; y < HEIGHT; y++) {
            for (isize x = 0; x < WIDTH; x++) {
                u8 byte = DATA[one * HEIGHT + y];
                if (byte & (0x80 >> x)) {
                    g.rect(Math::Recti{x, y - 8, 1, 1}.cast<f64>());
                }
            }
        }
    }

    f64 units() const override {
        return 8;
    }
};

} // namespace Karm::Media
