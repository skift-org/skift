#pragma once

#include <karm-text/run.h>

#include "base.h"

namespace Vaev::Paint {

struct Text : public Node {
    Math::Vec2f _topLeft;
    Strong<Karm::Text::Run> _run;

    Text(Math::Vec2f topLeft, Strong<Karm::Text::Run> run)
        : _topLeft(topLeft), _run(run) {
    }

    virtual void paint(Gfx::Canvas &) {
        // g.save();
        // g.translate(_topLeft + Math::Vec2f{0, _run->baseline()});
        // g.fillStyle(Gfx::BLACK);
        // _run->paint(g);
        // g.restore();
    }

    virtual void repr(Io::Emit &e) const {
        e("(text)");
    }
};

} // namespace Vaev::Paint
