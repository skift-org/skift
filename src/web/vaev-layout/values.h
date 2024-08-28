#pragma once

#include <vaev-base/color.h>
#include <vaev-base/width.h>

#include "base.h"

namespace Vaev::Layout {

Px resolve(Tree &t, Frag &f, Length l);

Px resolve(Tree &t, Frag &f, PercentOr<Length> p, Px relative);

Px resolve(Tree &t, Frag &f, Width w, Px relative);

} // namespace Vaev::Layout
