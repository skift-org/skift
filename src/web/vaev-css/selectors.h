#pragma once
#include "builder.h"

namespace Vaev::Css {

Style::Selector parseSelector(Slice<Sst> content);

} // namespace Vaev::Css
