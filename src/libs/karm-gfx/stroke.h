#pragma once

#include <karm-math/poly.h>

#include "path.h"
#include "style.h"

namespace Karm::Gfx {

void createStroke(Math::Polyf &poly, Path const &path, StrokeStyle stroke);

void createSolid(Math::Polyf &poly, Path &path);

} // namespace Karm::Gfx
