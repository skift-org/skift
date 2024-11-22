#pragma once

#include <karm-gfx/stroke.h>

#include "base.h"

namespace Karm::Scene {

struct Shape : public Node {
    Math::Path _path;
    Opt<Gfx::Stroke> _stroke;
    Opt<Gfx::Fill> _fill;
    Gfx::FillRule _rule = Gfx::FillRule::NONZERO;

    Shape(Math::Path path, Opt<Gfx::Stroke> stroke, Opt<Gfx::Fill> fill)
        : _path(path), _stroke(stroke), _fill(fill) {}

    Math::Rectf bound() override {
        return _path.bound();
    }

    void paint(Gfx::Canvas &g, Math::Rectf r) override {
        if (not bound().colide(r))
            return;

        g.path(_path);

        if (_fill)
            g.fill(*_fill, _rule);

        if (_stroke)
            g.stroke(*_stroke);
    }

    void repr(Io::Emit &e) const override {
        e("(shape z:{} {})", zIndex);
    }
};

} // namespace Karm::Scene
