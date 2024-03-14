#pragma once

#include "shape.h"

namespace Karm::Gfx {

void createStroke(Shape &shape, Path const &path, StrokeStyle stroke);

void createSolid(Shape &shape, Path &path);

} // namespace Karm::Gfx
