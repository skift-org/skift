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

static Css::Content expandVariables(Cursor<Css::Sst> &cursor, Map<String, Css::Content> computedVars) {
    // replacing the var with its value in the cascade
    Css::Content computedDecl = {};
    while (not cursor.ended()) {
        if (cursor.peek() == Css::Sst::FUNC and cursor.peek().prefix == Css::Token::function("var(")) {

            auto const varName = cursor->content[0].token.data;
            // if the variable is defined in the cascade
            if (computedVars.has(varName)) {
                computedDecl.pushBack(computedVars.get(varName));
            } else {
                if (cursor->content.len() > 2) {
                    // using the default value
                    Cursor<Css::Sst> cur = cursor->content;
                    cur.next(2);
                    eatWhitespace(cur);
                    auto defaultValue = expandVariables(cur, computedVars);
                    computedDecl.pushBack(defaultValue);
                } else {
                    // invalid property
                    logWarn("variable not found: {} {}", varName, cursor->content);
                }
            }
            cursor.next();
        } else if (cursor.peek() == Css::Sst::FUNC) {
            Cursor<Css::Sst> cur = cursor->content;
            computedDecl.pushBack(cursor.peek());
            computedDecl[computedDecl.len() - 1].content = expandVariables(cur, computedVars);
            cursor.next();
        } else {
            computedDecl.pushBack(cursor.peek());
            cursor.next();
        }
    }
    return computedDecl;
}

static Res<StyleProp> resolve(DeferredProp const &prop, Map<String, Css::Content> computedVars) {
    Cursor<Css::Sst> cursor = prop.value;

    auto computedDecl = expandVariables(cursor, computedVars);

    // building the declaration
    Css::Sst decl{Css::Sst::DECL};
    decl.token = Css::Token::ident(prop.propName);
    decl.content = computedDecl;

    // parsing the declaration
    Res<StyleProp> computed = parseDeclaration<StyleProp>(decl, false);
    if (not computed) {
        logWarn("failed to parse declaration: {}", computed.none());
    }
    return computed;
}

Strong<Computed> Computer::computeFor(Computed const &parent, Markup::Element const &el) {
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
        [](auto const &a, auto const &b) {
            return spec(a->selector) <=> spec(b->selector);
        }
    );

    // Get the style attribute if any
    auto styleAttr = el.getAttribute(Html::STYLE_ATTR);

    StyleRule styleRule{
        .selector = UNIVERSAL,
        .props = parseDeclarations<StyleProp>(styleAttr ? *styleAttr : ""),
    };
    matchingRules.pushBack(&styleRule);

    // Compute computed style
    auto computed = makeStrong<Computed>(Computed::initial());
    computed->inherit(parent);

    for (auto const &styleRule : matchingRules) {
        for (auto &prop : styleRule->props) {
            if (auto deferred = prop.is<DeferredProp>()) {
                auto resolved = resolve(prop.unwrap<DeferredProp>(), computed->variables.cow());

                if (not resolved) {
                    continue;
                }
                resolved.unwrap().apply(*computed);
            }
            if (prop.important == Important::NO)
                prop.apply(*computed);
        }
    }

    // TODO: We might want to find a better way to do that :^)
    for (auto const &styleRule : matchingRules) {
        for (auto const &prop : styleRule->props) {
            if (prop.important == Important::YES)
                prop.apply(*computed);
        }
    }

    return computed;
}

} // namespace Vaev::Style
