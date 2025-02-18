#pragma once

#include <vaev-dom/document.h>

#include "base.h"

namespace Vaev::Layout {

Box build(Style::Computer& c, Gc::Ref<Dom::Document> doc);

Box buildForPseudoElement(Rc<Style::Computed> style);

} // namespace Vaev::Layout
