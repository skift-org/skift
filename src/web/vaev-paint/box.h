#pragma once

#include <vaev-base/background.h>
#include <vaev-base/borders.h>

#include "base.h"

namespace Vaev::Paint {

struct Box : public Node {
    RectPx bound;
    Borders borders;
    Vec<Background> backgrounds;

    void paint(Gfx::Context &ctx) override {
        ColorContext cctx; // FIXME: Resolving color should happen in the layout phase
        for (auto &background : backgrounds)
            ctx._fillRect(bound.cast<isize>(), cctx.resolve(background.paint));
    }

    void repr(Io::Emit &e) const override {
        e("(box {})", bound);
    }
};

} // namespace Vaev::Paint
