#include "grid.h"

#include "block.h"

namespace Vaev::Layout {

Output gridLayout(Tree &t, Frag &f, Box box, Input input) {
    // FIXME: Implement grid layout
    return blockLayout(t, f, box, input);
}

} // namespace Vaev::Layout
