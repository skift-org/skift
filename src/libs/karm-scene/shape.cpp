module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:shape;

import Karm.Core;
import :node;

namespace Karm::Scene {

export struct Shape : Node {
    Math::Path _path;
    Opt<Gfx::Stroke> _stroke;
    Opt<Gfx::Fill> _fill;
    Gfx::FillRule _rule = Gfx::FillRule::NONZERO;

    Shape(Math::Path path, Opt<Gfx::Stroke> stroke, Opt<Gfx::Fill> fill)
        : _path(path), _stroke(stroke), _fill(fill) {}

    Math::Rectf bound() override {
        return _path.bound();
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions) override {
        if (not bound().colide(r))
            return;

        if (not _fill and not _stroke)
            return;

        g.push();
        g.beginPath();
        g.path(_path);
        if (_fill)
            g.fill(*_fill, _rule);
        if (_stroke)
            g.stroke(*_stroke);
        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(shape z:{} {})", zIndex, _path);
    }
};

} // namespace Karm::Scene
