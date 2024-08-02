#pragma once

#include <vaev-base/background.h>

#include "base.h"
#include "borders.h"

namespace Vaev::Paint {

struct Box : public Node {
    RectPx bound;
    Borders borders;
    Vec<Background> backgrounds;

    void paint(Gfx::Canvas &ctx) override {
        auto insetRadii = borders.paint(ctx, bound.cast<f64>());
        ColorContext colorContext; // FIXME: Resolving color should happen in the layout phase
        for (auto &background : backgrounds) {
            ctx.begin();
            ctx.rect(bound.cast<f64>(), insetRadii);
            ctx.fill(colorContext.resolve(background.fill));
        }
    }

    void repr(Io::Emit &e) const override {
        e("(box {})", bound);
    }
};

} // namespace Vaev::Paint
