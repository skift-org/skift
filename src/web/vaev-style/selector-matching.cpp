#include "selector-matching.h"

namespace Vaev::Style {

static constexpr bool DEBUG_MATCHING = false;

// https://www.w3.org/TR/selectors-4/#descendant-combinators
static bool _matchDescendant(Selector const& s, Markup::Element const& e) {
    Cursor<Markup::Node> curr = &e;
    while (curr->hasParent()) {
        auto& parent = curr->parentNode();
        if (auto el = parent.is<Markup::Element>())
            if (matchSelector(s, *el))
                return true;
        curr = &parent;
    }
    return false;
}

// https://www.w3.org/TR/selectors-4/#child-combinators
static bool _matchChild(Selector const& s, Markup::Element const& e) {
    if (not e.hasParent())
        return false;

    auto& parent = e.parentNode();
    if (auto el = parent.is<Markup::Element>())
        return matchSelector(s, *el);
    return false;
}

// https://www.w3.org/TR/selectors-4/#adjacent-sibling-combinators
static bool _matchAdjacent(Selector const& s, Markup::Element const& e) {
    if (not e.hasPreviousSibling())
        return false;

    auto prev = e.previousSibling();
    if (auto el = prev.is<Markup::Element>())
        return matchSelector(s, *el);
    return false;
}

// https://www.w3.org/TR/selectors-4/#general-sibling-combinators
static bool _matchSubsequent(Selector const& s, Markup::Element const& e) {
    Cursor<Markup::Node> curr = &e;
    while (curr->hasPreviousSibling()) {
        auto prev = curr->previousSibling();
        if (auto el = prev.is<Markup::Element>())
            if (matchSelector(s, *el))
                return true;
        curr = &prev.unwrap();
    }
    return false;
}

static bool _match(Infix const& s, Markup::Element const& e) {
    if (not matchSelector(s.rhs.unwrap(), e))
        return false;

    switch (s.type) {
    case Infix::DESCENDANT: // ' '
        return _matchDescendant(*s.lhs, e);

    case Infix::CHILD: // >
        return _matchChild(*s.lhs, e);

    case Infix::ADJACENT: // +
        return _matchAdjacent(*s.lhs, e);

    case Infix::SUBSEQUENT: // ~
        return _matchSubsequent(*s.lhs, e);

    default:
        logWarnIf(DEBUG_MATCHING, "unimplemented selector: {}", s);
        return false;
    }
}

static bool _match(Nfix const& s, Markup::Element const& el) {
    switch (s.type) {
    case Nfix::AND:
        for (auto& inner : s.inners)
            if (not matchSelector(inner, el))
                return false;
        return true;

    // 4.1. Selector Lists
    // https://www.w3.org/TR/selectors-4/#grouping
    // and
    // 4.2. The Matches-Any Pseudo-class: :is()
    // https://www.w3.org/TR/selectors-4/#matchess
    case Nfix::OR:
        for (auto& inner : s.inners)
            if (matchSelector(inner, el))
                return true;
        return false;

    case Nfix::NOT:
        return not matchSelector(s.inners[0], el);

    case Nfix::WHERE:
        return not matchSelector(s.inners[0], el);

    default:
        logWarnIf(DEBUG_MATCHING, "unimplemented selector: {}", s);
        return false;
    }
}

// 5.1. Type (tag name) selector
// https://www.w3.org/TR/selectors-4/#type
static bool _match(TypeSelector const& s, Markup::Element const& el) {
    return el.tagName == s.type;
}

static bool _match(IdSelector const& s, Markup::Element const& el) {
    return el.id() == s.id;
}

static bool _match(ClassSelector const& s, Markup::Element const& el) {
    return el.classList.contains(s.class_);
}

// 8.2. The Link History Pseudo-classes: :link and :visited
// https://www.w3.org/TR/selectors-4/#link

static bool _matchLink(Markup::Element const& el) {
    return el.tagName == Html::A and el.hasAttribute(Html::HREF_ATTR);
}

// 14.3.3. :first-child pseudo-class
// https://www.w3.org/TR/selectors-4/#the-first-child-pseudo

static bool _matchFirstChild(Markup::Element const& e) {
    Cursor<Markup::Node> curr = &e;
    while (curr->hasPreviousSibling()) {
        auto prev = curr->previousSibling();
        if (auto el = prev.is<Markup::Element>())
            return false;
        curr = &prev.unwrap();
    }
    return true;
}

// 14.3.4. :last-child pseudo-class
// https://www.w3.org/TR/selectors-4/#the-last-child-pseudo

static bool _matchLastChild(Markup::Element const& e) {
    Cursor<Markup::Node> curr = &e;
    while (curr->hasNextSibling()) {
        auto next = curr->nextSibling();
        if (auto el = next.is<Markup::Element>())
            return false;
        curr = &next.unwrap();
    }
    return true;
}

// 14.4.3. :first-of-type pseudo-class
// https://www.w3.org/TR/selectors-4/#the-first-of-type-pseudo

static bool _matchFirstOfType(Markup::Element const& e) {
    Cursor<Markup::Node> curr = &e;
    auto tag = e.tagName;

    while (curr->hasPreviousSibling()) {
        auto prev = curr->previousSibling();
        if (auto el = prev.is<Markup::Element>())
            if (e.tagName == tag)
                return false;
        curr = &prev.unwrap();
    }
    return true;
}

// 14.4.4. :last-of-type pseudo-class
// https://www.w3.org/TR/selectors-4/#the-last-of-type-pseudo

static bool _matchLastOfType(Markup::Element const& e) {
    Cursor<Markup::Node> curr = &e;
    auto tag = e.tagName;

    while (curr->hasNextSibling()) {
        auto prev = curr->nextSibling();
        if (auto el = prev.is<Markup::Element>())
            if (e.tagName == tag)
                return false;
        curr = &prev.unwrap();
    }
    return true;
}

static bool _match(Pseudo const& s, Markup::Element const& el) {
    switch (s.type) {
    case Pseudo::LINK:
        return _matchLink(el);

    case Pseudo::ROOT:
        return el.tagName == Html::HTML;

    case Pseudo::FIRST_OF_TYPE:
        return _matchFirstOfType(el);

    case Pseudo::LAST_OF_TYPE:
        return _matchLastOfType(el);

    case Pseudo::FIRST_CHILD:
        return _matchFirstChild(el);

    case Pseudo::LAST_CHILD:
        return _matchLastChild(el);

    default:
        logDebugIf(DEBUG_MATCHING, "unimplemented pseudo class: {}", s);
        return false;
    }
}

// 5.2. Universal selector
// https://www.w3.org/TR/selectors-4/#the-universal-selector
static bool _match(UniversalSelector const&, Markup::Element const&) {
    return true;
}

// MARK: Selector --------------------------------------------------------------

Opt<Spec> matchSelectorWithSpecificity(Selector const& selector, Markup::Element const& el) {
    return selector.visit(Visitor{
        [&](Nfix const& n) -> Opt<Spec> {
            if (n.type == Nfix::OR) {
                Opt<Spec> specificity;
                for (auto& inner : n.inners) {
                    if (matchSelector(inner, el))
                        specificity = max(specificity, spec(inner));
                }
                return specificity;
            }
            return matchSelector(n, el) ? Opt<Spec>{spec(n)} : NONE;
        },
        [&](auto const& s) -> Opt<Spec> {
            return matchSelector(s, el) ? Opt<Spec>{spec(s)} : NONE;
        }
    });
}

bool matchSelector(Selector const& selector, Markup::Element const& el) {
    // Route the selector to the appropriate matching function.

    return selector.visit(Visitor{
        [&](auto const& s) {
            if constexpr (requires { _match(s, el); })
                return _match(s, el);

            logWarnIf(DEBUG_MATCHING, "unimplemented selector: {}", s);
            return false;
        }
    });
}

} // namespace Vaev::Style
