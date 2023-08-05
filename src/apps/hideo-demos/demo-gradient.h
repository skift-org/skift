#pragma once

#include <karm-ui/view.h>

#include "base.h"

namespace Demos {

static inline Demo GRADIENT_DEMO{
    Mdi::GRADIENT_HORIZONTAL,
    "Gradients",
    "Gradients rendering",
    []() {
        return Ui::canvas(
            [](Gfx::Context &g, Math::Vec2i bound) {
                // Red, orange, yellow, green, blue, indigo, violet
                auto colors = Gfx::Gradient::linear()
                                  .withColors(Gfx::RED, Gfx::ORANGE, Gfx::YELLOW, Gfx::GREEN, Gfx::BLUE, Gfx::INDIGO, Gfx::VIOLET)
                                  .bake();

                g.fillStyle(colors);
                g.fill(bound);

                g.fillStyle(Gfx::WHITE);
                g.fill({16, 26}, "Graphic design is my passion");
            });
    },
};

} // namespace Demos
