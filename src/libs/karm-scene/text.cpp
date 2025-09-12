module;

#include <karm-gfx/prose.h>

export module Karm.Scene:text;

import :node;

namespace Karm::Scene {

export struct Text : Node {
    Math::Vec2f _origin;
    Rc<Gfx::Prose> _prose;

    Text(Math::Vec2f origin, Rc<Gfx::Prose> prose)
        : _origin(origin), _prose(prose) {}

    Math::Rectf bound() override {
        return {_origin, _prose->size().cast<f64>()};
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions) override {
        if (not bound().colide(r))
            return;

        g.push();
        g.origin(_origin);
        g.fill(*_prose);
        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(text z:{} {})", zIndex, _origin);
    }
};

} // namespace Karm::Scene
