#include "grid.h"

#include "block.h"

namespace Vaev::Layout {

Output gridLayout(Tree &tree, Box &box, Input input) {
    // FIXME: Implement grid layout
    return blockLayout(tree, box, input, 0, NONE);
}

} // namespace Vaev::Layout
