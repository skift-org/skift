#pragma once

#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static inline Demo GRADIENT_DEMO{
    Mdi::GRADIENT_HORIZONTAL,
    "Gradients",
    "Gradients rendering",
    [] {
        return Ui::canvas(
            [](Gfx::Canvas &g, Math::Vec2i bound) {
                // Red, orange, yellow, green, blue, indigo, violet
                auto colors = Gfx::Gradient::linear()
                                  .withColors(Gfx::RED, Gfx::ORANGE, Gfx::YELLOW, Gfx::GREEN, Gfx::BLUE, Gfx::INDIGO, Gfx::VIOLET)
                                  .bake();

                g.fillStyle(colors);
                g.fill(bound);
            }
        );
    },
};

} // namespace Hideo::Demos
