#pragma once

#include "base.h"

namespace Vaev::Layout {

InsetsPx computeMargins(Tree &t, Frag &f, Input input);

InsetsPx computeBorders(Tree &t, Frag &f);

Output layout(Tree &t, Frag &f, Input input);

} // namespace Vaev::Layout
