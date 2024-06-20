#include "select.h"

namespace Vaev::Style {

// MARK: Selector Specificity ---------------------------------------------------

Spec spec(Selector const &) {
    // TODO: Implement
    return Spec::ZERO;
}

// MARK: Selector Matching -----------------------------------------------------

bool match(Selector const &sel, Dom::Element const &el);

bool _match(Infix const &s, Dom::Element const &) {
    switch (s.type) {
    default:
        notImplemented();
    }
}

bool _match(Nfix const &s, Dom::Element const &el) {
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
bool _match(TypeSelector const &s, Dom::Element const &el) {
    return el.tagName == s.type;
}

bool _match(IdSelector const &s, Dom::Element const &el) {
    return el.id() == s.id;
}

bool _match(ClassSelector const &s, Dom::Element const &el) {
    return el.classList.contains(s.class_);
}

// 5.2. Universal selector
// https://www.w3.org/TR/selectors-4/#the-universal-selector
bool _match(UniversalSelector const &, Dom::Element const &) {
    return true;
}

bool match(Selector const &sel, Dom::Element const &el) {
    return sel.visit(
        [&](auto const &s) {
            if constexpr (requires { _match(s, el); }) {
                return _match(s, el);
            } else {
                logWarn("unimplemented selector: {}", s);
                return false;
            }
        }
    );
}

} // namespace Vaev::Style
