#pragma once

#include "box.h"
#include "context.h"

namespace Vaev::Layout {

Output blockLayout(Context &ctx, Box box, Input input);

Px blockMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

} // namespace Vaev::Layout
