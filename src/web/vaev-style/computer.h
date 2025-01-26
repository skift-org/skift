#pragma once

#include <vaev-markup/dom.h>

#include "computed.h"
#include "stylesheet.h"

namespace Vaev::Style {

struct Computer {
    Media _media;
    StyleBook const& _styleBook;

    using MatchingRules = Vec<Tuple<Cursor<StyleRule>, Spec>>;

    void _evalRule(Rule const& rule, Markup::Element const& el, MatchingRules& matches);

    void _evalRule(Rule const& rule, Page const& page, PageComputedStyle& c);

    Rc<Computed> _evalCascade(Computed const& parent, MatchingRules& matches);

    Rc<Computed> computeFor(Computed const& parent, Markup::Element const& el);

    Rc<PageComputedStyle> computeFor(Computed const& parent, Page const& page);
};

} // namespace Vaev::Style
