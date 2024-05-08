#include "select.h"

namespace Web::Select {

Spec match(Selector const &sel, Dom::Element &el);

Spec match(Combinator const &s, Dom::Element &el) {
    switch (s.type) {
    // 4.1. Selector Lists
    // https://www.w3.org/TR/selectors-4/#grouping
    case Combinator::LIST: {
        Spec result = Spec::NOMATCH;
        for (auto &inner : s.inners)
            result = result or match(inner, el);
        return result;
    }

    // 4.2. The Matches-Any Pseudo-class: :is()
    // https://www.w3.org/TR/selectors-4/#matches
    case Combinator::IS: {
        Spec result = Spec::NOMATCH;
        for (auto &inner : s.inners) {
            if (auto sel = match(inner, el); sel > result)
                result = sel;
        }
        return result + Spec::B;
    }

    // 4.3. The Negation (Matches-None) Pseudo-class: :not()
    // https://www.w3.org/TR/selectors-4/#negation
    case Combinator::NOT: {
        Spec result = {0, 0, 0};
        for (auto &inner : s.inners)
            result = result and not match(inner, el);
        return result + Spec::B;
    }

    // 4.4. The Specificity-adjustment Pseudo-class: :where()
    // https://www.w3.org/TR/selectors-4/#zero-matches
    case Combinator::WHERE: {
        Spec result = Spec::NOMATCH;
        for (auto &inner : s.inners)
            result = result or match(inner, el);
        return result ? Spec::ZERO : Spec::NOMATCH;
    }

    default:
        notImplemented();
    }
}

// 5.1. Type (tag name) selector
// https://www.w3.org/TR/selectors-4/#type
Spec match(TypeSelector const &s, Dom::Element &el) {
    if (el.tagName == s.type)
        return Spec::C;
    return Spec::NOMATCH;
}

// 5.2. Universal selector
// https://www.w3.org/TR/selectors-4/#the-universal-selector
Spec match(UniversalSelector const &, Dom::Element &) {
    return Spec::ZERO;
}

Spec match(Selector const &sel, Dom::Element &el) {
    return sel.visit(Visitor{
        [&](Combinator const &s) -> Spec {
            return match(s, el);
        },
        [&](TypeSelector const &s) -> Spec {
            return match(s, el);
        },
        [&](UniversalSelector const &s) -> Spec {
            return match(s, el);
        },
        [&](auto &) -> Spec {
            notImplemented();
        }
    });
}

} // namespace Web::Select
