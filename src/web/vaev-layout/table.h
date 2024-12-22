#pragma once

#include "input_output.h"
#include "tree.h"

namespace Vaev::Layout {

Output tableLayout(Tree &tree, Box &box, Input input, usize startAt, Opt<usize> stopAt);

} // namespace Vaev::Layout
