#include <vaev-style/decls.h>

#include "computer.h"

namespace Vaev::Style {

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
        },
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
        },
    });
}

void Computer::_evalRule(Rule const& rule, Vec<FontFace>& fontFaces) {
    rule.visit(Visitor{
        [&](FontFaceRule const& r) {
            auto& fontFace = fontFaces.emplaceBack();
            for (auto const& decl : r.descs)
                decl.apply(fontFace);
        },
        [&](MediaRule const& r) {
            if (r.match(_media))
                for (auto const& subRule : r.rules)
                    _evalRule(subRule, fontFaces);
        },
        [&](auto const&) {
            // Ignore other rule types
        },
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

void Computer::loadFontFaces() {
    for (auto const& sheet : _styleBook.styleSheets) {

        Vec<FontFace> fontFaces;
        for (auto const& rule : sheet.rules)
            _evalRule(rule, fontFaces);

        for (auto const& ff : fontFaces) {
            for (auto const& src : ff.sources) {
                if (src.identifier.is<Mime::Url>()) {
                    auto fontUrl = src.identifier.unwrap<Mime::Url>();

                    auto resolvedUrl = Mime::Url::resolveReference(sheet.href, fontUrl);

                    if (not resolvedUrl) {
                        logWarn("Cannot resolve urls when loading fonts: {} {}", fontUrl, sheet.href);
                        continue;
                    }

                    // FIXME: use attrs from style::FontFace
                    if (fontBook.load(resolvedUrl.unwrap()))
                        break;

                    logWarn("Failed to load font at {}", resolvedUrl);
                } else {
                    if (
                        fontBook.queryExact(Text::FontQuery{.family = src.identifier.unwrap<Text::Family>()})
                    )
                        break;

                    logWarn("Failed to assets font {}", src.identifier.unwrap<Text::Family>());
                }
            }
        }
    }
}

} // namespace Vaev::Style
