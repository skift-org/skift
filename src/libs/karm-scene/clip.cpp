module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:clip;

import Karm.Core;
import :proxy;

namespace Karm::Scene {

export struct Clip : Proxy {
    Union<Math::Path, Math::Rectf> _clipArea;
    Gfx::FillRule _rule;

    Clip(Rc<Node> node, Math::Path path, Gfx::FillRule rule = Gfx::FillRule::NONZERO)
        : Proxy(node), _clipArea(path), _rule(rule) {}

    Clip(Rc<Node> node, Math::Rectf rect)
        : Proxy(node), _clipArea(rect), _rule(Gfx::FillRule::NONZERO) {}

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        g.push();

        if (auto path = _clipArea.is<Math::Path>()) {
            g.beginPath();
            g.path(*path);
            g.clip(_rule);
        } else if (auto rect = _clipArea.is<Math::Rectf>()) {
            g.clip(*rect);
        }

        Proxy::paint(g, r, o);

        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(clip content:{})", _node);
    }
};

} // namespace Karm::Scene
