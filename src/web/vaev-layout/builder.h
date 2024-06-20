#pragma once

#include <vaev-dom/node.h>
#include <vaev-style/computer.h>

#include "block.h"

namespace Vaev::Layout {

void build(Style::Computer &c, Dom::Node const &n, Flow &parent);

} // namespace Vaev::Layout
