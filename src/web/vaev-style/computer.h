#pragma once

#include <vaev-dom/node.h>

#include "computed.h"
#include "stylesheet.h"

namespace Vaev::Style {

struct Computer {
    StyleBook const &_styleBook;

    Strong<Computed> computeFor(Dom::Element const &el);
};

} // namespace Vaev::Style
