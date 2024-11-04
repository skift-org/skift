#pragma once

#include <karm-text/prose.h>

#include "base.h"

namespace Karm::Scene {

struct Text : public Node {
    Math::Vec2f origin;
    Strong<Karm::Text::Prose> prose;

    Text(Math::Vec2f origin, Strong<Karm::Text::Prose> prose)
        : origin(origin), prose(prose) {}

    void paint(Gfx::Canvas &g) override {
        g.push();
        g.origin(origin);
        prose->paint(g);
        g.pop();
    }

    void repr(Io::Emit &e) const override {
        e("(text z:{} {})", zIndex, origin);
    }
};

} // namespace Karm::Scene
