#include "computer.h"
#include <vaev-style/decls.h>

namespace Vaev::Style {

Computed const& Computed::initial() {
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

void Computer::_evalRule(Rule const& rule, Gc::Ref<Dom::Element> el, MatchingRules& matches) {
    rule.visit(Visitor{
        [&](StyleRule const& r) {
            if (auto specificity = r.match(el))
                matches.pushBack({&r, specificity.unwrap()});
        },
        [&](MediaRule const& r) {
            if (r.match(_media))
                for (auto const& subRule : r.rules)
                    _evalRule(subRule, el, matches);
        },
        [&](auto const&) {
            // Ignore other rule types
        }
    });
}

void Computer::_evalRule(Rule const& rule, Page const& page, PageComputedStyle& c) {
    rule.visit(Visitor{
        [&](PageRule const& r) {
            if (r.match(page))
                r.apply(c);
        },
        [&](MediaRule const& r) {
            if (r.match(_media))
                for (auto const& subRule : r.rules)
                    _evalRule(subRule, page, c);
        },
        [&](auto const&) {
            // Ignore other rule types
        }
    });
}

Rc<Computed> Computer::_evalCascade(Computed const& parent, MatchingRules& matchingRules) {
    // Sort origin and specificity
    stableSort(
        matchingRules,
        [](auto const& a, auto const& b) {
            if (a.v0->origin != b.v0->origin)
                return a.v0->origin <=> b.v0->origin;
            return a.v1 <=> b.v1;
        }
    );

    // Compute computed style
    auto computed = makeRc<Computed>(Computed::initial());
    computed->inherit(parent);
    Vec<Cursor<StyleProp>> importantProps;

    // HACK: Apply custom properties first
    for (auto const& [styleRule, _] : matchingRules) {
        for (auto& prop : styleRule->props) {
            if (prop.is<CustomProp>())
                prop.apply(parent, *computed);
        }
    }

    for (auto const& [styleRule, _] : matchingRules) {
        for (auto& prop : styleRule->props) {
            if (not prop.is<CustomProp>()) {
                if (prop.important == Important::NO)
                    prop.apply(parent, *computed);
                else
                    importantProps.pushBack(&prop);
            }
        }
    }

    for (auto const& prop : iterRev(importantProps))
        prop->apply(parent, *computed);

    return computed;
}

// https://drafts.csswg.org/css-cascade/#cascade-origin
Rc<Computed> Computer::computeFor(Computed const& parent, Gc::Ref<Dom::Element> el) {
    MatchingRules matchingRules;

    // Collect matching styles rules
    for (auto const& sheet : _styleBook.styleSheets)
        for (auto const& rule : sheet.rules)
            _evalRule(rule, el, matchingRules);

    // Get the style attribute if any
    auto styleAttr = el->getAttribute(Html::STYLE_ATTR);

    StyleRule styleRule{
        .props = parseDeclarations<StyleProp>(styleAttr ? *styleAttr : ""),
        .origin = Origin::INLINE,
    };
    matchingRules.pushBack({&styleRule, INLINE_SPEC});

    return _evalCascade(parent, matchingRules);
}

Rc<PageComputedStyle> Computer::computeFor(Computed const& parent, Page const& page) {
    auto computed = makeRc<PageComputedStyle>(parent);

    for (auto const& sheet : _styleBook.styleSheets)
        for (auto const& rule : sheet.rules)
            _evalRule(rule, page, *computed);

    return computed;
}

} // namespace Vaev::Style
