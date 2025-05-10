#pragma once

#include "base.h"

namespace Karm::Scene {

struct Clip : public Stack {
    Math::Path _path;
    Gfx::FillRule _rule;

    Clip() = delete;

    Clip(Math::Path path, Gfx::FillRule rule = Gfx::FillRule::NONZERO)
        : Stack(), _path(path), _rule(rule) {}

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        g.push();

        g.beginPath();
        g.path(_path);
        g.clip(_rule);

        Stack::paint(g, r, o);

        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(clip");
        if (_children) {
            e.indentNewline();
            for (auto& child : _children) {
                child->repr(e);
                e.newline();
            }
            e.deindent();
        }
        e(")");
    }
};

} // namespace Karm::Scene
