#pragma once

#include <vaev-markup/dom.h>

#include "computed.h"
#include "stylesheet.h"

namespace Vaev::Style {

struct Computer {
    Media _media;
    StyleBook const &_styleBook;

    using MatchingRules = Vec<Cursor<StyleRule>>;

    void _evalRule(Rule const &rule, Markup::Element const &el, MatchingRules &matches);

    void _evalRule(Rule const &rule, Page const &page, PageComputedStyle &c);

    Strong<Computed> _evalCascade(Computed const &parent, MatchingRules &matches);

    Strong<Computed> computeFor(Computed const &parent, Markup::Element const &el);

    Strong<PageComputedStyle> computeFor(Page const &page);
};

} // namespace Vaev::Style
