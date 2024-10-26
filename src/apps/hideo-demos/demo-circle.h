#pragma once

#include <karm-ui/view.h>
#include <mdi/circle.h>

#include "base.h"

namespace Hideo::Demos {

static inline Demo CIRCLE_DEMO{
    Mdi::CIRCLE,
    "Circles",
    "Circles rendering",
    [] {
        return Ui::canvas(
            [](Gfx::Canvas &g, Math::Vec2i size) {
                Math::Rand rand{};

                for (isize i = 0; i < 10; i++) {
                    f64 s = rand.nextInt(4, 10);
                    s *= s;

                    g.beginPath();
                    g.ellipse({
                        rand.nextVec2(size).cast<f64>(),
                        s,
                    });

                    g.strokeStyle(
                        Gfx::stroke(Gfx::randomColor(rand))
                            .withWidth(rand.nextInt(2, s))
                    );
                    g.stroke();
                }
            }
        );
    },
};

} // namespace Hideo::Demos
