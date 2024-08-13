#include "grid.h"

#include "block.h"

namespace Vaev::Layout {

void gridLayout(Context &ctx, Box box) {
    // FIXME: Implement grid layout
    blockLayout(ctx, box);
}

Px gridMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px availableSpace) {
    // FIXME: Implement grid measure
    return blockMeasure(ctx, axis, intrinsic, availableSpace);
}

} // namespace Vaev::Layout
