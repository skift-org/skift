#pragma once

#include "box.h"
#include "context.h"

namespace Vaev::Layout {

void tableLayout(Context &ctx, Box box);

Px tableMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

} // namespace Vaev::Layout
