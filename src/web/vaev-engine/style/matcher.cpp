module;

#include <karm-logger/logger.h>

export module Vaev.Engine:style.matcher;

import Karm.Core;
import Karm.Gc;
import :dom;
import :style.selector;

using namespace Karm;

namespace Vaev::Style {

static constexpr bool DEBUG_MATCHING = false;

static bool _matchSelector(Selector const& selector, Gc::Ref<Dom::Element> el);
export Opt<Spec> matchSelector(Selector const& selector, Gc::Ref<Dom::Element> el);

// https://www.w3.org/TR/selectors-4/#descendant-combinators
static bool _matchDescendant(Selector const& s, Gc::Ref<Dom::Element> e) {
    Gc::Ptr<Dom::Node> curr = e;
    while (curr->hasParentNode()) {
        auto parent = curr->parentNode();
        if (auto el = parent->is<Dom::Element>())
            if (matchSelector(s, *el))
                return true;
        curr = parent;
    }
    return false;
}

// https://www.w3.org/TR/selectors-4/#child-combinators
static bool _matchChild(Selector const& s, Gc::Ref<Dom::Element> e) {
    if (not e->hasParentNode())
        return false;

    auto parent = e->parentNode();
    if (auto el = parent->is<Dom::Element>())
        return _matchSelector(s, *el);
    return false;
}

// https://www.w3.org/TR/selectors-4/#adjacent-sibling-combinators
static bool _matchAdjacent(Selector const& s, Gc::Ref<Dom::Element> e) {
    if (not e->hasPreviousSibling())
        return false;

    auto prev = e->previousSibling();
    if (auto el = prev->is<Dom::Element>())
        return _matchSelector(s, *el);
    return false;
}

// https://www.w3.org/TR/selectors-4/#general-sibling-combinators
static bool _matchSubsequent(Selector const& s, Gc::Ref<Dom::Element> e) {
    Gc::Ptr<Dom::Node> curr = e;
    while (curr->hasPreviousSibling()) {
        auto prev = curr->previousSibling();
        if (auto el = prev->is<Dom::Element>())
            if (_matchSelector(s, *el))
                return true;
        curr = prev;
    }
    return false;
}

static bool _match(Infix const& s, Gc::Ref<Dom::Element> e) {
    if (not _matchSelector(s.rhs.unwrap(), e))
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

static bool _match(Nfix const& s, Gc::Ref<Dom::Element> el) {
    switch (s.type) {
    case Nfix::AND:
        for (auto& inner : s.inners)
            if (not _matchSelector(inner, el))
                return false;
        return true;

    // 4.1. Selector Lists
    // https://www.w3.org/TR/selectors-4/#grouping
    // and
    // 4.2. The Matches-Any Pseudo-class: :is()
    // https://www.w3.org/TR/selectors-4/#matchess
    case Nfix::OR:
        for (auto& inner : s.inners)
            if (_matchSelector(inner, el))
                return true;
        return false;

    case Nfix::NOT:
        return not _matchSelector(s.inners[0], el);

    case Nfix::WHERE:
        return not _matchSelector(s.inners[0], el);

    default:
        logWarnIf(DEBUG_MATCHING, "unimplemented selector: {}", s);
        return false;
    }
}

// 5.1. Type (tag name) selector
// https://www.w3.org/TR/selectors-4/#type
static bool _match(TypeSelector const& s, Gc::Ref<Dom::Element> el) {
    return s.qualifiedName.match(el->qualifiedName);
}

static bool _match(IdSelector const& s, Gc::Ref<Dom::Element> el) {
    return el->id() == s.id;
}

static bool _match(ClassSelector const& s, Gc::Ref<Dom::Element> el) {
    return el->classList.contains(s.class_);
}

// 6. Attribute Selector
// https://www.w3.org/TR/selectors-4/#attribute-selectors
static bool _match(AttributeSelector const& s, Gc::Ref<Dom::Element> el) {
    // TODO: What should we do if there are multiple attributes
    //       with the same name but different namespaces?
    auto maybeAttrValue = s.qualifiedName.isWildcard()
                              ? el->getAttributeUnqualified(s.qualifiedName.name.unwrap("unexpected wildcard attribute selector"))
                              : el->getAttribute(s.qualifiedName.fullyQualified());

    if (s.match == AttributeSelector::PRESENT) {
        // Represents an element with the att attribute, whatever the value of the attribute.
        return maybeAttrValue != NONE;
    }

    if (not maybeAttrValue)
        return false;

    auto attrValue = maybeAttrValue.unwrap();

    auto cmp = [&s](Rune const& a, Rune const& b) {
        if (s.case_ == AttributeSelector::INSENSITIVE)
            return toAsciiLower(a) == toAsciiLower(b);
        else
            return a == b;
    };

    if (s.match == AttributeSelector::EXACT) {
        // Represents an element with the att attribute whose value is exactly "val".
        return startWith(attrValue, s.value.str(), cmp) == Match::YES;
    } else if (s.match == AttributeSelector::CONTAINS) {
        // If "val" contains whitespace, it will never represent anything (since the words are separated by spaces).
        if (contains(s.value, ' '))
            return false;

        // Also if "val" is the empty string, it will never represent anything.
        if (s.value.len() == 0)
            return false;

        // Represents an element with the att attribute whose value is a whitespace-separated list of words,
        // one of which is exactly "val".
        for (auto piece : split(attrValue, ' ')) {
            if (piece.len() == 0)
                continue;

            if (startWith(piece, s.value, cmp) == Match::YES)
                return true;
        }

        return false;
    } else if (s.match == AttributeSelector::HYPHENATED) {
        // Represents an element with the att attribute, its value either being exactly "val"
        auto prefixMatch = startWith(attrValue, s.value.str(), cmp);
        if (prefixMatch == Match::YES)
            return true;
        if (prefixMatch == Match::NO)
            return false;

        // or beginning with "val" immediately followed by "-" (U+002D).
        return attrValue[s.value.len()] == '-';
    } else {
        // 6.2. Substring matching attribute selectors
        // https://www.w3.org/TR/selectors-4/#attribute-substrings

        // If "val" is the empty string then the selector does not represent anything.
        if (s.value.len() == 0)
            return false;

        if (s.match == AttributeSelector::STR_START_WITH) {
            // Represents an element with the att attribute whose value begins with the prefix "val".
            return startWith(attrValue, s.value.str(), cmp) != Match::NO;
        } else if (s.match == AttributeSelector::STR_END_WITH) {
            // Represents an element with the att attribute whose value ends with the suffix "val".
            return endWith(attrValue, s.value.str(), cmp) != Match::NO;
        } else {
            // Represents an element with the att attribute whose value contains at least one instance of the
            // substring "val".
            return contains(attrValue, s.value, cmp);
        }
    }
}

// 8.2. The Link History Pseudo-classes: :link and :visited
// https://www.w3.org/TR/selectors-4/#link

static bool _matchLink(Gc::Ref<Dom::Element> el) {
    return el->qualifiedName == Html::A_TAG and el->hasAttribute(Html::HREF_ATTR);
}

// 14.3.1. :nth-child() pseudo-class
// 14.3.2. :nth-last-child() pseudo-class
// 14.3.3. :first-child pseudo-class
// 14.3.4. :last-child pseudo-class
// https://www.w3.org/TR/selectors-4/#the-nth-child-pseudo
// https://www.w3.org/TR/selectors-4/#the-nth-last-child-pseudo
// https://www.w3.org/TR/selectors-4/#the-first-child-pseudo
// https://www.w3.org/TR/selectors-4/#the-last-child-pseudo
static bool _matchNthChild(Gc::Ref<Dom::Element> e, Pseudo::AnBofS const& anbOfS, bool isLast) {
    auto [anb, ofS] = anbOfS;
    if (ofS) {
        if (not matchSelector(*(ofS.unwrap()), *e))
            return false;

        auto filterFunc = [&](Gc::Ptr<Dom::Node> node) {
            auto el = node->is<Dom::Element>();
            return el ? matchSelector(*(ofS.unwrap()), *el) != NONE : false;
        };
        auto index = isLast ? e->reverseIndex(filterFunc) : e->index(filterFunc);
        return anb.match(index + 1);
    } else {
        auto filterFunc = [&](Gc::Ptr<Dom::Node> node) {
            return node->is<Dom::Element>() != NONE;
        };
        auto index = isLast ? e->reverseIndex(filterFunc) : e->index(filterFunc);
        return anb.match(index + 1);
    }
}

// 14.4.1. :nth-of-type pseudo-class
// 14.4.2. :nth-last-of-type pseudo-class
// 14.4.3. :first-of-type pseudo-class
// 14.4.4. :last-of-type pseudo-class
// https://www.w3.org/TR/selectors-4/#the-nth-of-type-pseudo
// https://www.w3.org/TR/selectors-4/#the-nth-last-of-type-pseudo
// https://www.w3.org/TR/selectors-4/#the-first-of-type-pseudo
// https://www.w3.org/TR/selectors-4/#the-last-of-type-pseudo
static bool _matchNthOfType(Gc::Ref<Dom::Element> e, AnB const& anb, bool isLast) {
    Gc::Ptr<Dom::Node> curr = e;
    auto name = e->qualifiedName;

    auto filterFunc = [&](Gc::Ptr<Dom::Node> node) {
        auto el = node->is<Dom::Element>();
        return el ? el->qualifiedName == name : false;
    };

    auto index = isLast ? e->reverseIndex(filterFunc) : e->index(filterFunc);
    return anb.match(index + 1);
}

static bool _match(Pseudo const& s, Gc::Ref<Dom::Element> el) {
    switch (s.type) {
    case Pseudo::LINK:
        return _matchLink(el);

    case Pseudo::ROOT:
        return el->qualifiedName == Html::HTML_TAG;

    case Pseudo::FIRST_OF_TYPE:
        return _matchNthOfType(el, AnB{0, 1}, false);

    case Pseudo::LAST_OF_TYPE:
        return _matchNthOfType(el, AnB{0, 1}, true);

    case Pseudo::FIRST_CHILD:
        return _matchNthChild(el, Pseudo::AnBofS{AnB{0, 1}, NONE}, false);

    case Pseudo::LAST_CHILD:
        return _matchNthChild(el, Pseudo::AnBofS{AnB{0, 1}, NONE}, true);

    case Pseudo::NTH_CHILD:
        return _matchNthChild(el, s.extra.unwrap<Pseudo::AnBofS>("unexpected missing AnB"), false);

    case Pseudo::NTH_LAST_CHILD:
        return _matchNthChild(el, s.extra.unwrap<Pseudo::AnBofS>("unexpected missing AnB"), true);

    case Pseudo::NTH_OF_TYPE:
        return _matchNthOfType(el, s.extra.unwrap<Pseudo::AnBofS>("unexpected missing AnB").v0, false);

    case Pseudo::NTH_LAST_OF_TYPE:
        return _matchNthOfType(el, s.extra.unwrap<Pseudo::AnBofS>("unexpected missing AnB").v0, true);

    default:
        logDebugIf(DEBUG_MATCHING, "unimplemented pseudo class: {}", s);
        return false;
    }
}

// MARK: Selector --------------------------------------------------------------

static bool _matchSelector(Selector const& selector, Gc::Ref<Dom::Element> el) {
    // Route the selector to the appropriate matching function.
    return selector.visit(Visitor{[&](auto const& s) {
        if constexpr (requires { _match(s, el); })
            return _match(s, el);

        logWarnIf(DEBUG_MATCHING, "unimplemented selector: {}", s);
        return false;
    }});
}

export Opt<Spec> matchSelector(Selector const& selector, Gc::Ref<Dom::Element> el) {
    if (auto n = selector.is<Nfix>(); n and n->type == Nfix::OR) {
        Opt<Spec> specificity;
        for (auto& inner : n->inners) {
            if (_matchSelector(inner, el))
                specificity = max(specificity, spec(inner));
        }
        return specificity;
    }

    if (_matchSelector(selector, el))
        return spec(selector);

    return NONE;
}

} // namespace Vaev::Style
