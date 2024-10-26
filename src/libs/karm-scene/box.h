#pragma once

#include <karm-gfx/borders.h>

#include "base.h"

namespace Karm::Scene {

struct Box : public Node {
    Math::Rectf bound;
    Gfx::Borders borders;
    Vec<Gfx::Fill> backgrounds;

    void paint(Gfx::Canvas &ctx) override {
        for (auto &background : backgrounds) {
            ctx.beginPath();
            auto radii = borders.radii.reduceOverlap(bound.size());
            ctx.rect(bound, radii);
            ctx.fill(background);
        }

        borders.paint(ctx, bound);
    }

    void repr(Io::Emit &e) const override {
        e("(box z:{} {} {} {})", zIndex, bound, borders.radii, backgrounds);
    }
};

} // namespace Karm::Scene
