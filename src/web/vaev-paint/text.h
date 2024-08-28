#pragma once

#include <karm-text/run.h>

#include "base.h"

namespace Vaev::Paint {

struct Text : public Node {
    Math::Vec2f baseline;
    Strong<Karm::Text::Run> run;
    Gfx::Fill fill;

    Text(Math::Vec2f baseline, Strong<Karm::Text::Run> run, Gfx::Fill fill)
        : baseline(baseline), run(run), fill(fill) {}

    virtual void paint(Gfx::Canvas &g) {
        g.push();
        g.fillStyle(fill);
        g.fill(run->_font, *run, baseline);

        // g.beginPath();
        // g.line(Math::Edgef{baseline, baseline + Math::Vec2f{run->_width, 0}});
        // g.stroke(Gfx::Stroke{
        //     .fill = Gfx::PINK,
        //     .width = 1,
        // });
        g.pop();
    }

    virtual void repr(Io::Emit &e) const {
        e("(text {})", baseline);
    }
};

} // namespace Vaev::Paint
