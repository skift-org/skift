#pragma once

#include <karm-text/book.h>
#include <vaev-dom/element.h>

#include "computed.h"
#include "stylesheet.h"

namespace Vaev::Style {

struct Computer {
    Media _media;
    StyleBook const& _styleBook;
    Text::FontBook& fontBook;

    using MatchingRules = Vec<Tuple<Cursor<StyleRule>, Spec>>;

    void _evalRule(Rule const& rule, Gc::Ref<Dom::Element> el, MatchingRules& matches);

    void _evalRule(Rule const& rule, Page const& page, PageComputedStyle& c);

    void _evalRule(Rule const& rule, Vec<FontFace>& fontFaces);

    Rc<Computed> _evalCascade(Computed const& parent, MatchingRules& matches);

    Rc<Computed> computeFor(Computed const& parent, Gc::Ref<Dom::Element> el);

    Rc<PageComputedStyle> computeFor(Computed const& parent, Page const& page);

    void loadFontFaces();
};

} // namespace Vaev::Style
