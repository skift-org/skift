module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:clear;

import Karm.Core;
import :proxy;

namespace Karm::Scene {

export struct Clear : Proxy {
    Gfx::Color color;

    Clear(Rc<Node> node, Gfx::Color color)
        : Proxy(node), color(color) {}

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        if (not bound().colide(r))
            return;

        if (o.showBackgroundGraphics) {
            g.clear(color.blendOver(Gfx::WHITE));
        }
        _node->paint(g, r, o);
    }

    void repr(Io::Emit& e) const override {
        e("(clear z:{} {} {})", zIndex, color, _node);
    }
};

} // namespace Karm::Scene
