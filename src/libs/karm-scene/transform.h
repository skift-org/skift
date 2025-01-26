#pragma once

#include "base.h"

namespace Karm::Scene {

struct Transform : public Node {
    Rc<Node> _content;
    Math::Trans2f _transform;

    Transform(Rc<Node> content, Math::Trans2f transform)
        : _content(content), _transform(transform) {}

    void prepare() override {
        _content->prepare();
    }

    Math::Rectf bound() override {
        return _transform
            .apply(_content->bound())
            .bound();
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        if (not bound().colide(r))
            return;

        g.push();
        g.transform(_transform);
        r = _transform.inverse().apply(r).bound();
        _content->paint(g, r, o);
        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(transform transform:{} content:{})", _transform, _content);
    }
};

} // namespace Karm::Scene
