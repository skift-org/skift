#pragma once

#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

#include "block.h"

namespace Vaev::Layout {

void build(Style::Computer &c, Dom::Node const &n, Flow &parent);

Strong<Frag> build(Style::Computer &c, Dom::Document const &doc);

} // namespace Vaev::Layout
