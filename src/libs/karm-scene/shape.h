#pragma once

#include <karm-gfx/stroke.h>

#include "base.h"

namespace Karm::Scene {

struct Shape : public Node {
    Math::Path path;
    Opt<Gfx::Stroke> stroke;
    Opt<Gfx::Fill> fill;
    Gfx::FillRule rule = Gfx::FillRule::NONZERO;

    Shape(Math::Path path, Opt<Gfx::Stroke> stroke, Opt<Gfx::Fill> fill)
        : path(path), stroke(stroke), fill(fill) {}

    void paint(Gfx::Canvas &g) override {
        g.path(path);

        if (fill)
            g.fill(*fill, rule);

        if (stroke)
            g.stroke(*stroke);
    }

    void repr(Io::Emit &e) const override {
        e("(shape z:{} {})", zIndex);
    }
};

} // namespace Karm::Scene
