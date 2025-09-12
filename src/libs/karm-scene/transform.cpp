module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:transform;

import Karm.Core;
import :proxy;

namespace Karm::Scene {

export struct Transform : Proxy {
    Math::Trans2f _transform;

    Transform(Rc<Node> node, Math::Trans2f transform)
        : Proxy(node), _transform(transform) {}

    Math::Rectf bound() override {
        return _transform
            .apply(_node->bound())
            .bound();
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        if (not bound().colide(r))
            return;

        g.push();
        g.transform(_transform);
        r = _transform.inverse().apply(r).bound();
        _node->paint(g, r, o);
        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(transform transform:{} content:{})", _transform, _node);
    }
};

} // namespace Karm::Scene
