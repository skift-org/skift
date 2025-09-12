module;

#include <karm-gfx/font.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>

export module Vaev.Engine:style.computer;

import Karm.Gc;
import Karm.Font;
import Karm.Ref;

import :dom;
import :style.specified;
import :style.stylesheet;

namespace Vaev::Style {

export struct Computer {
    Media _media;
    StyleSheetList const& _styleBook;
    Font::Database& fontBook;
    StyleRuleLookup _styleRuleLookup{};

    // MARK: Cascading ---------------------------------------------------------

    void _evalRule(Rule const& rule, Page const& page, PageSpecifiedValues& c) {
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

    void _evalRule(Rule const& rule, Vec<FontFace>& fontFaces) {
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

    Rc<SpecifiedValues> _evalCascade(SpecifiedValues const& parent, MatchingRules& matchingRules) {
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
        auto computed = makeRc<SpecifiedValues>(SpecifiedValues::initial());
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

        for (auto const& prop : importantProps)
            prop->apply(parent, *computed);

        return computed;
    }

    // MARK: Computing ---------------------------------------------------------

    static Rc<Gfx::Fontface> _lookupFontface(Font::Database& db, Style::SpecifiedValues& style) {
        Font::Query fq{
            .weight = style.font->weight,
            .style = style.font->style.val,
        };

        for (auto family : style.font->families) {
            if (auto font = db.queryClosest(family.name, fq))
                return font.unwrap();
        }

        if (auto font = db.queryClosest("system"_sym))
            return font.unwrap();

        return Gfx::Fontface::fallback();
    }

    // https://svgwg.org/specs/integration/#svg-css-sizing
    static void _applySVGElementSizingRules(Gc::Ref<Dom::Element> svgEl, Vec<Style::StyleProp>& styleProps) {
        if (auto parentEl = svgEl->parentNode()->is<Dom::Element>()) {
            // **If we have an <svg> element inside a CSS context**
            if (parentEl->qualifiedName.ns == Svg::NAMESPACE)
                return;

            // To resolve 'auto' value on ‘svg’ element if the ‘viewBox’ attribute is not specified:
            // - ...
            // - If any of the sizing attributes are missing, resolve the missing ‘svg’ element width to '300px' and missing
            // height to '150px' (using CSS 2.1 replaced elements size calculation).
            if (not svgEl->hasAttribute(Svg::VIEW_BOX_ATTR)) {
                if (not svgEl->hasAttribute(Svg::WIDTH_ATTR)) {
                    styleProps.pushBack(WidthProp{CalcValue<PercentOr<Length>>{PercentOr<Length>{Length{Au{300}}}}});
                }
                if (not svgEl->hasAttribute(Svg::HEIGHT_ATTR)) {
                    styleProps.pushBack(HeightProp{CalcValue<PercentOr<Length>>{PercentOr<Length>{Length{Au{150}}}}});
                }
            }
        }
    }

    // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
    Vec<Style::StyleProp> _considerPresentationAttributes(Gc::Ref<Dom::Element> el) {
        if (el->qualifiedName.ns != Svg::NAMESPACE)
            return {};

        Vec<Style::StyleProp> styleProps;
        for (auto [attr, attrValue] : el->attributes.iter()) {
            parseSVGPresentationAttribute(attr.name, attrValue->value, styleProps);
        }

        if (el->qualifiedName == Svg::SVG_TAG)
            _applySVGElementSizingRules(el, styleProps);

        return styleProps;
    }

    // https://drafts.csswg.org/css-cascade/#cascade-origin
    Rc<SpecifiedValues> computeFor(SpecifiedValues const& parent, Gc::Ref<Dom::Element> el) {
        MatchingRules matchingRules = _styleRuleLookup.buildMatchingRules(el);

        // Get the style attribute if any
        auto styleAttr = el->style();

        StyleRule styleRule{
            .props = parseDeclarations<StyleProp>(styleAttr ? *styleAttr : ""),
            .origin = Origin::INLINE,
        };
        matchingRules.pushBack({&styleRule, INLINE_SPEC});

        // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
        // Presentation attributes contribute to the author level of the cascade, followed by all other author-level
        // style sheets, and have specificity 0.
        StyleRule presentationAttributes{
            .props = _considerPresentationAttributes(el),
            .origin = Origin::AUTHOR,
        };
        matchingRules.pushBack({&presentationAttributes, PRESENTATION_ATTR_SPEC});

        return _evalCascade(parent, matchingRules);
    }

    Rc<PageSpecifiedValues> computeFor(SpecifiedValues const& parent, Page const& page) {
        auto computed = makeRc<PageSpecifiedValues>(parent);

        for (auto const& sheet : _styleBook.styleSheets)
            for (auto const& rule : sheet.rules)
                _evalRule(rule, page, *computed);

        for (auto& area : computed->_areas) {
            auto font = _lookupFontface(fontBook, *area.specifiedValues());
            area.specifiedValues()->fontFace = font;
        }

        return computed;
    }

    // MARK: Styling -----------------------------------------------------------

    void styleElement(SpecifiedValues const& parentSpecifiedValues, Dom::Element& el) {
        auto specifiedValues = computeFor(parentSpecifiedValues, el);
        el._specifiedValues = specifiedValues;

        // HACK: This is basically nonsense to avoid doing too much font lookup,
        //       and it should be remove once the style engine get refactored
        //       and computed values are properly handled.
        if (not parentSpecifiedValues.font.sameInstance(specifiedValues->font) and
            (parentSpecifiedValues.font->families != specifiedValues->font->families or
             parentSpecifiedValues.font->weight != specifiedValues->font->weight)) {
            auto font = _lookupFontface(fontBook, *specifiedValues);
            specifiedValues->fontFace = font;
        } else {
            specifiedValues->fontFace = parentSpecifiedValues.fontFace;
        }

        for (auto child = el.firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->is<Dom::Element>())
                styleElement(*specifiedValues, *childEl);
        }
    }

    void styleDocument(Dom::Document& doc) {
        if (auto el = doc.documentElement()) {
            auto rootSpecifiedValues = makeRc<SpecifiedValues>(SpecifiedValues::initial());
            rootSpecifiedValues->fontFace = _lookupFontface(fontBook, *rootSpecifiedValues);
            styleElement(*rootSpecifiedValues, *el);
        }
    }

    void build() {
        for (auto const& sheet : _styleBook.styleSheets) {
            for (auto const& rule : sheet.rules) {
                _addRuleToLookup(&rule);
            }
        }

        _loadFontFaces();
    }

    void _addRuleToLookup(Cursor<Rule> rule) {
        rule->visit(Visitor{
            [&](StyleRule const& r) {
                _styleRuleLookup.add(r);
            },
            [&](MediaRule const& r) {
                if (r.match(_media))
                    for (auto const& subRule : r.rules)
                        _addRuleToLookup(&subRule);
            },
            [&](auto const&) {
                // Ignore other rule types
            },
        });
    }

    void _loadFontFaces() {
        for (auto const& sheet : _styleBook.styleSheets) {

            Vec<FontFace> fontFaces;
            for (auto const& rule : sheet.rules)
                _evalRule(rule, fontFaces);

            for (auto const& ff : fontFaces) {
                for (auto const& src : ff.sources) {
                    if (src.identifier.is<Ref::Url>()) {
                        auto fontUrl = src.identifier.unwrap<Ref::Url>();

                        auto resolvedUrl = Ref::Url::resolveReference(sheet.href, fontUrl);

                        if (not resolvedUrl) {
                            logWarn("Cannot resolve urls when loading fonts: {} {}", ff.family, sheet.href);
                            continue;
                        }

                        // FIXME: use attrs from style::FontFace
                        if (fontBook.load(resolvedUrl.unwrap()))
                            break;

                        logWarn("Failed to load font {}", ff.family);
                    } else {
                        if (
                            fontBook.queryExact(src.identifier.unwrap<FontFamily>().name)
                        )
                            break;

                        logWarn("Failed to assets font {}", src.identifier.unwrap<FontFamily>().name);
                    }
                }
            }
        }
    }
};

} // namespace Vaev::Style
