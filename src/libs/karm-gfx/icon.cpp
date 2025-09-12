#include "icon.h"

import Mdi;

namespace Karm::Gfx {

void Icon::fill(Gfx::Canvas& g, Math::Vec2f pos, isize size) const {
    g.push();
    g.beginPath();
    g.origin(pos.cast<f64>());
    g.scale(size / (f64)this->size);
    auto p = Math::Path::fromSvg(path);
    g.path(p);
    g.fill();
    g.pop();
}

void Icon::stroke(Gfx::Canvas& g, Math::Vec2f pos, isize size) const {
    g.push();
    g.beginPath();
    g.origin(pos.cast<f64>());
    g.scale(size / (f64)this->size);
    g.path(Math::Path::fromSvg(path));
    g.stroke();
    g.pop();
}

} // namespace Karm::Gfx
