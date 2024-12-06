#include "computer.h"
#include <vaev-style/decls.h>

namespace Vaev::Style {

Computed const &Computed::initial() {
    static Computed computed = [] {
        Computed res{};
        StyleProp::any([&]<typename T>(Meta::Type<T>) {
            if constexpr (requires { T::initial(); })
                T{}.apply(res);
        });
        return res;
    }();
    return computed;
}

void Computer::_evalRule(Rule const &rule, Markup::Element const &el, MatchingRules &matches) {
    rule.visit(Visitor{
        [&](StyleRule const &r) {
            if (r.match(el))
                matches.pushBack(&r);
        },
        [&](MediaRule const &r) {
            if (r.match(_media))
                for (auto const &subRule : r.rules)
                    _evalRule(subRule, el, matches);
        },
        [&](auto const &) {
            // Ignore other rule types
        }
    });
}

void Computer::_evalRule(Rule const &rule, Page const &page, PageComputedStyle &c) {
    rule.visit(Visitor{
        [&](PageRule const &r) {
            if (r.match(page))
                r.apply(c);
        },
        [&](MediaRule const &r) {
            if (r.match(_media))
                for (auto const &subRule : r.rules)
                    _evalRule(subRule, page, c);
        },
        [&](auto const &) {
            // Ignore other rule types
        }
    });
}

Strong<Computed> Computer::_evalCascade(Computed const &parent, MatchingRules &matchingRules) {
    // Sort origin and specificity
    stableSort(
        matchingRules,
        [](auto const &a, auto const &b) {
            if (a->origin != b->origin)
                return a->origin <=> b->origin;
            return spec(a->selector) <=> spec(b->selector);
        }
    );

    // Compute computed style
    auto computed = makeStrong<Computed>(Computed::initial());
    computed->inherit(parent);
    Vec<Cursor<StyleProp>> importantProps;

    // HACK: Apply custom properties first
    for (auto const &styleRule : matchingRules) {
        for (auto &prop : styleRule->props) {
            if (prop.is<CustomProp>())
                prop.apply(parent, *computed);
        }
    }

    for (auto const &styleRule : matchingRules) {
        for (auto &prop : styleRule->props) {
            if (not prop.is<CustomProp>()) {
                if (prop.important == Important::NO)
                    prop.apply(parent, *computed);
                else
                    importantProps.pushBack(&prop);
            }
        }
    }

    for (auto const &prop : iterRev(importantProps))
        prop->apply(parent, *computed);

    return computed;
}

// https://drafts.csswg.org/css-cascade/#cascade-origin
Strong<Computed> Computer::computeFor(Computed const &parent, Markup::Element const &el) {
    MatchingRules matchingRules;

    // Collect matching styles rules
    for (auto const &sheet : _styleBook.styleSheets)
        for (auto const &rule : sheet.rules)
            _evalRule(rule, el, matchingRules);

    // Get the style attribute if any
    auto styleAttr = el.getAttribute(Html::STYLE_ATTR);

    StyleRule styleRule{
        .props = parseDeclarations<StyleProp>(styleAttr ? *styleAttr : ""),
        .origin = Origin::INLINE,
    };
    matchingRules.pushBack(&styleRule);

    return _evalCascade(parent, matchingRules);
}

Strong<PageComputedStyle> Computer::computeFor(Page const &page) {
    auto computed = makeStrong<PageComputedStyle>();

    for (auto const &sheet : _styleBook.styleSheets)
        for (auto const &rule : sheet.rules)
            _evalRule(rule, page, *computed);

    return computed;
}

} // namespace Vaev::Style
