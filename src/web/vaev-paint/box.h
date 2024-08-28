#pragma once

#include <vaev-base/background.h>

#include "base.h"

namespace Vaev::Paint {

struct Box : public Node {
    Math::Rectf bound;
    Math::Radiif radii;
    Vec<Gfx::Fill> backgrounds;

    void paint(Gfx::Canvas &ctx) override {
        for (auto &background : backgrounds) {
            ctx.beginPath();
            ctx.rect(bound.cast<f64>(), radii);
            ctx.fill(background);
        }
    }

    void repr(Io::Emit &e) const override {
        e("(box {})", bound);
    }
};

} // namespace Vaev::Paint
