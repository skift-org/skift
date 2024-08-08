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

    virtual void paint(Gfx::Canvas &g) {
        auto baseline = _run->baseline();
        g.push();
        g.fillStyle(Gfx::BLACK);
        g.fill(_run->_font, *_run, _topLeft + Math::Vec2f{0, baseline});
        g.pop();
    }

    virtual void repr(Io::Emit &e) const {
        e("(text {})", _topLeft);
    }
};

} // namespace Vaev::Paint
