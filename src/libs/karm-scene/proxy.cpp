module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:proxy;

import Karm.Core;
import :node;

namespace Karm::Scene {

export struct Proxy : Node {
    Rc<Node> _node;

    Proxy(Rc<Node> node)
        : _node(node) {}

    void prepare() override {
        _node->prepare();
    }

    Math::Rectf bound() override {
        return _node->bound();
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        _node->paint(g, r, o);
    }

    void repr(Io::Emit& e) const override {
        e("(proxy z:{} {})", zIndex, _node);
    }
};

} // namespace Karm::Scene
