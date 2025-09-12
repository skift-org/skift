module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:viewbox;

import Karm.Core;
import :proxy;

namespace Karm::Scene {

export struct Viewbox : Proxy {
    Math::Rectf _viewbox;

    Viewbox(Rc<Node> node, Math::Rectf viewbox)
        : Proxy(node), _viewbox(viewbox) {}

    Math::Rectf bound() override {
        return _viewbox.size();
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        if (not bound().colide(r))
            return;

        g.push();
        g.origin(_viewbox.xy);
        g.clip(_viewbox);
        Proxy::paint(g, r, o);
        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(viewbox {} content:{})", _viewbox, _node);
    }
};

} // namespace Karm::Scene
