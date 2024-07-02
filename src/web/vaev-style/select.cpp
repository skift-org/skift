#include "select.h"

namespace Vaev::Style {

// MARK: Selector Specificity ---------------------------------------------------

// https://www.w3.org/TR/selectors-3/#specificity
Spec spec(Selector const &s) {
    return s.visit(Visitor{
        [](Nfix const &n) {
            Spec sum = Spec::ZERO;
            for (auto &inner : n.inners)
                sum = sum + spec(inner);
            return sum;
        },
        [](Infix const &i) {
            return spec(*i.lhs) + spec(*i.rhs);
        },
        [](UniversalSelector const &) {
            return Spec::ZERO;
        },
        [](EmptySelector const &) {
            return Spec::ZERO;
        },
        [](IdSelector const &) {
            return Spec::A;
        },
        [](TypeSelector const &) {
            return Spec::C;
        },
        [](ClassSelector const &) {
            return Spec::B;
        },
        [](PseudoClass const &) {
            return Spec::ZERO;
        },
        [](AttributeSelector const &) {
            return Spec::ZERO;
        }
    });
}

// MARK: Selector Matching -----------------------------------------------------

bool _match(Infix const &s, Dom::Element const &e) {
    switch (s.type) {
    // https://www.w3.org/TR/selectors-4/#descendant-combinators
    case Infix::Type::DESCENDANT: {
        Dom::Node const *curr = &e;
        while (curr->hasParent()) {
            auto &parent = curr->parentNode();
            if (auto *el = parent.is<Dom::Element>())
                if (s.lhs->match(*el))
                    return true;
            curr = &parent;
        }
        return false;
    } break;

    // https://www.w3.org/TR/selectors-4/#child-combinators
    case Infix::Type::CHILD: {
        if (not e.hasParent())
            return false;

        auto &parent = e.parentNode();
        if (auto *el = parent.is<Dom::Element>())
            return s.lhs->match(*el);
        return false;
    }

    // https://www.w3.org/TR/selectors-4/#adjacent-sibling-combinators
    case Infix::Type::ADJACENT: {
        if (not e.hasPreviousSibling())
            return false;

        auto prev = e.previousSibling();
        if (auto *el = prev.is<Dom::Element>())
            return s.lhs->match(*el);
        return false;
    }

    // https://www.w3.org/TR/selectors-4/#general-sibling-combinators
    case Infix::Type::SUBSEQUENT: {
        Dom::Node const *curr = &e;
        while (curr->hasPreviousSibling()) {
            auto prev = curr->previousSibling();
            if (auto *el = prev.is<Dom::Element>())
                if (s.lhs->match(*el))
                    return true;
            curr = &prev.unwrap();
        }
        return false;
    }

    // https://www.w3.org/TR/selectors-4/#the-column-combinator
    case Infix::Type::COLUMN:
    default:
        unreachable();
    }
}

bool _match(Nfix const &s, Dom::Element const &el) {
    switch (s.type) {
    case Nfix::AND:
        for (auto &inner : s.inners)
            if (not inner.match(el))
                return false;
        return true;

    // 4.1. Selector Lists
    // https://www.w3.org/TR/selectors-4/#grouping
    // and
    // 4.2. The Matches-Any Pseudo-class: :is()
    // https://www.w3.org/TR/selectors-4/#matchess
    case Nfix::OR:
        for (auto &inner : s.inners)
            if (inner.match(el))
                return true;
        return false;

    case Nfix::NOT:
        return not s.inners[0].match(el);

    case Nfix::WHERE:
        return not s.inners[0].match(el);

    default:
        unreachable();
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

// MARK: Selector -------- -----------------------------------------------------

bool Selector::match(Dom::Element const &el) const {
    return visit(
        [&](auto const &s) {
            if constexpr (requires { _match(s, el); })
                return _match(s, el);

            logWarn("unimplemented selector: {}", s);
            return false;
        }
    );
}
} // namespace Vaev::Style
