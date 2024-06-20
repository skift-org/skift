#include "computer.h"

namespace Vaev::Style {

Strong<Computed> Computer::computeFor(Dom::Element const &el) {
    Vec<StyleRule const *> matchingRules;

    // Collect matching styles rules
    for (auto const &sheet : _styleBook.styleSheets) {
        for (auto const &rule : sheet.rules) {
            if (
                auto const *styleRule = rule.is<StyleRule>();
                styleRule and match(styleRule->selector, el)
            ) {
                matchingRules.pushBack(styleRule);
            }
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
            prop.apply(*computed);
        }
    }

    return computed;
}

} // namespace Vaev::Style
