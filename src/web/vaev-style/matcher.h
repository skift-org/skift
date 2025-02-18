#pragma once

#include <karm-base/vec.h>
#include <vaev-dom/element.h>

#include "selector.h"

namespace Vaev::Style {

Opt<Spec> matchSelector(Selector const& selector, Gc::Ref<Dom::Element> el);

} // namespace Vaev::Style
