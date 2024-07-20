#pragma once

#include <vaev-dom/node.h>

#include "computed.h"
#include "stylesheet.h"

namespace Vaev::Style {

struct Computer {
    Media _media;
    StyleBook const &_styleBook;

    using MatchingRules = Vec<StyleRule const *>;

    void _evalRule(Rule const &rule, Dom::Element const &el, MatchingRules &matches);

    Strong<Computed> computeFor(Computed const &parent, Dom::Element const &el);
};

} // namespace Vaev::Style
