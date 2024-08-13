#pragma once

#include "box.h"
#include "context.h"

namespace Vaev::Layout {

void flexLayout(Context &ctx, Box box);

Px flexMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

} // namespace Vaev::Layout
