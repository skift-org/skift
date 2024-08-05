#pragma once

#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static inline Str CAT = {
#include "cat.path"
};

static inline Demo SVG_DEMO{
    Mdi::SVG,
    "SVG",
    "SVG rendering",
    [] {
        return Ui::canvas(
            [](Gfx::Context &g, ...) {
                g.beginPath();
                g.evalSvg(CAT);

                g.fillStyle(Ui::GRAY700);
                g.fill();

                g.strokeStyle(Gfx::stroke(Gfx::BLUE).withWidth(1).withAlign(Gfx::OUTSIDE_ALIGN));
                g.stroke();
            }
        );
    },
};

} // namespace Hideo::Demos
