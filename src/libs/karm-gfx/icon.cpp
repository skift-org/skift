#include "icon.h"

namespace Karm::Gfx {

void Icon::fill(Gfx::Canvas &g, Math::Vec2i pos) const {
    g.push();
    g.beginPath();
    g.origin(pos.cast<f64>());
    g.scale(_size / _icon.width);
    auto path = Math::Path::fromSvg(svg());
    g.path(path);
    g.fill();
    g.pop();
}

void Icon::stroke(Gfx::Canvas &g, Math::Vec2i pos) const {

    g.push();
    g.beginPath();
    g.origin(pos.cast<f64>());
    g.scale(_size / _icon.width);
    auto path = Math::Path::fromSvg(svg());
    g.path(path);
    g.stroke();
    g.pop();
}

} // namespace Karm::Gfx
