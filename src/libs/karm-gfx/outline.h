#pragma once

#include "borders.h"

namespace Karm::Gfx {

struct Outline {
    f64 width;
    f64 offset;
    Fill fill;
    BorderStyle style;

    void paint(Gfx::Canvas& c, Math::Rectf rect, Math::Radiif radii);
};

} // namespace Karm::Gfx
