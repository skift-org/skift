#pragma once

#include "box.h"
#include "context.h"

namespace Vaev::Layout {

void gridLayout(Context &ctx, Box box);

Px gridMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

} // namespace Vaev::Layout
