#pragma once

#include "base.h"

namespace Vaev::Layout {

InsetsPx computeMargins(Tree &tree, Box &box, Input input);

InsetsPx computeBorders(Tree &tree, Box &box);

Output layout(Tree &tree, Box &box, Input input);

} // namespace Vaev::Layout
