#pragma once

#include <karm-gfx/borders.h>

#include "base.h"

namespace Karm::Scene {

struct Box : public Node {
    Math::Rectf _bound;
    Gfx::Borders _borders;
    Vec<Gfx::Fill> _backgrounds;

    Box(Math::Rectf bound, Gfx::Borders borders, Vec<Gfx::Fill> backgrounds)
        : _bound(bound), _borders(borders), _backgrounds(backgrounds) {
    }

    Math::Rectf bound() override {
        return _bound;
    }

    void paint(Gfx::Canvas& ctx, Math::Rectf r, PaintOptions o) override {
        if (not r.colide(bound()))
            return;

        if (o.showBackgroundGraphics) {
            for (auto& background : _backgrounds) {
                ctx.beginPath();
                auto radii = _borders.radii.reduceOverlap(_bound.size());
                ctx.rect(_bound, radii);
                ctx.fill(background);
            }
        }

        _borders.paint(ctx, _bound);
    }

    void repr(Io::Emit& e) const override {
        e("(box z:{} {} {} {})", zIndex, _bound, _borders.radii, _backgrounds);
    }
};

} // namespace Karm::Scene
