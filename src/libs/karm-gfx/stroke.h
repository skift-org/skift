#pragma once

#include <karm-math/shape.h>

#include "path.h"

namespace Karm::Gfx {

void createStroke(Math::Shapef &shape, Path const &path, StrokeStyle stroke);

void createSolid(Math::Shapef &shape, Path &path);

} // namespace Karm::Gfx
