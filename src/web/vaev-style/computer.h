#pragma once

#include <vaev-markup/dom.h>

#include "computed.h"
#include "stylesheet.h"

namespace Vaev::Style {

struct Computer {
    Media _media;
    StyleBook const &_styleBook;

    using MatchingRules = Vec<StyleRule const *>;

    void _evalRule(Rule const &rule, Markup::Element const &el, MatchingRules &matches);

    Strong<Computed> computeFor(Computed const &parent, Markup::Element const &el);
};

} // namespace Vaev::Style
