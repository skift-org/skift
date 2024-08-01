#pragma once

#include <vaev-base/background.h>
#include <vaev-base/borders.h>

#include "base.h"

namespace Vaev::Paint {

struct Box : public Node {
    RectPx bound;
    Borders borders;
    Vec<Background> backgrounds;

    void paint(Gfx::Canvas &ctx) override {
        ColorContext colorContext; // FIXME: Resolving color should happen in the layout phase
        for (auto &background : backgrounds) {
            ctx.begin();
            ctx.rect(bound.cast<f64>());
            ctx.fill(colorContext.resolve(background.fill));
        }
    }

    void repr(Io::Emit &e) const override {
        e("(box {})", bound);
    }
};

} // namespace Vaev::Paint
