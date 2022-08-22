#pragma once

#include <karm-base/array.h>
#include <karm-gfx/context.h>

#include "font.h"

namespace Karm::Media {

struct VgaFont : public Font {
    static constexpr int WIDTH = 8;
    static constexpr int HEIGHT = 8;

    static constexpr Array<uint8_t, 1024> const DATA = {
#include "font-vga.inc"
    };

    VgaFont() = default;

    ~VgaFont() = default;

    FontMetrics metrics() const override {
        return {
            .ascend = 12,
            .captop = 10,
            .descend = 4,
            .linegap = 4,
            .advance = 8,
        };
    }

    double advance(Rune) const override {
        return 8;
    }

    void fillRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        One<Ibm437> one;
        encodeOne<Ibm437>(rune, one);

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                uint8_t byte = DATA[one * HEIGHT + y];
                if (byte & (0x80 >> x)) {
                    g.plot(baseline + Math::Vec2i{x, y - HEIGHT}, g.textStyle().color);
                }
            }
        }
    }
};

} // namespace Karm::Media
