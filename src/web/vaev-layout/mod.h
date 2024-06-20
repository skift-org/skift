#pragma once

#include <vaev-dom/node.h>

#include "base.h"

namespace Vaev::Layout {

/// Convert a DOM tree from the bottom-up into a layout tree.
void build(Dom::Node const &n, Vec<Strong<Frag>> &out);

} // namespace Vaev::Layout
