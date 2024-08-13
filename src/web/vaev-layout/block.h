#pragma once

#include "box.h"
#include "context.h"

namespace Vaev::Layout {

void blockLayout(Context &ctx, Box box);

Px blockMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

} // namespace Vaev::Layout
