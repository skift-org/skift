#include "grid.h"

#include "block.h"

namespace Vaev::Layout {

Output gridLayout(Context &ctx, Box box, Input input) {
    // FIXME: Implement grid layout
    return blockLayout(ctx, box, input);
}

} // namespace Vaev::Layout
