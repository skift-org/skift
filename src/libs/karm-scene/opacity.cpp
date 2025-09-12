module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:opacity;

import Karm.Core;
import :proxy;

namespace Karm::Scene {

export struct Opacity : Proxy {
    f64 _opacity;

    Opacity(Rc<Node> node, f64 opacity)
        : Proxy(node), _opacity(opacity) {}

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        g.push();
        g.opacity(_opacity);
        _node->paint(g, r, o);
        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(opacity opacity:{} content:{})", _opacity, _node);
    }
};

} // namespace Karm::Scene
