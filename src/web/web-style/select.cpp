#include "select.h"

namespace Web::Style {

bool match(Selector const &sel, Dom::Element &el);

bool match(Infix const &s, Dom::Element &) {
    switch (s.type) {
    default:
        notImplemented();
    }
}

bool match(Nfix const &s, Dom::Element &el) {
    switch (s.type) {
    case Nfix::AND:
        for (auto &inner : s.inners)
            if (not match(inner, el))
                return false;
        return true;

    // 4.1. Selector Lists
    // https://www.w3.org/TR/selectors-4/#grouping
    // and
    // 4.2. The Matches-Any Pseudo-class: :is()
    // https://www.w3.org/TR/selectors-4/#matchess
    case Nfix::OR:
        for (auto &inner : s.inners)
            if (match(inner, el))
                return true;
        return false;

    case Nfix::NOT:
        return not match(s.inners[0], el);

    case Nfix::WHERE:
        return match(s.inners[0], el);

    default:
        notImplemented();
    }
}

// 5.1. Type (tag name) selector
// https://www.w3.org/TR/selectors-4/#type
bool match(TypeSelector const &s, Dom::Element &el) {
    return el.tagName == s.type;
}

bool match(IdSelector const &s, Dom::Element &el) {
    return el.id() == s.id;
}

bool match(ClassSelector const &s, Dom::Element &el) {
    return el.classList.contains(s.class_);
}

// 5.2. Universal selector
// https://www.w3.org/TR/selectors-4/#the-universal-selector
bool match(UniversalSelector const &, Dom::Element &) {
    return true;
}

bool match(Selector const &sel, Dom::Element &el) {
    return sel.visit(Visitor{
        [&](Infix const &s) {
            return match(s, el);
        },
        [&](TypeSelector const &s) {
            return match(s, el);
        },
        [&](UniversalSelector const &s) {
            return match(s, el);
        },
        [&](ClassSelector const &s) {
            return match(s, el);
        },
        [&](IdSelector const &s) {
            return match(s, el);
        },
        [&](auto &) -> bool {
            notImplemented();
        }
    });
}

} // namespace Web::Style
