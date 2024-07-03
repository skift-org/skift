#include "computer.h"

namespace Vaev::Style {

void Computer::_evalRule(Rule const &rule, Dom::Element const &el, MatchingRules &matches) {
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

Strong<Computed> Computer::computeFor(Dom::Element const &el) {
    MatchingRules matchingRules;

    // Collect matching styles rules
    for (auto const &sheet : _styleBook.styleSheets) {
        for (auto const &rule : sheet.rules) {
            _evalRule(rule, el, matchingRules);
        }
    }

    // Sort rules by specificity
    stableSort(
        matchingRules,
        [](auto const *a, auto const *b) {
            return spec(a->selector) <=> spec(b->selector);
        }
    );

    // Compute computed style
    auto computed = makeStrong<Computed>();
    for (auto const *styleRule : matchingRules) {
        for (auto const &prop : styleRule->props) {
            if (prop.important == Important::NO)
                prop.apply(*computed);
        }
    }

    // TODO: We might want to find a better way to do that :^)
    for (auto const *styleRule : matchingRules) {
        for (auto const &prop : styleRule->props) {
            if (prop.important == Important::YES)
                prop.apply(*computed);
        }
    }

    return computed;
}

} // namespace Vaev::Style
