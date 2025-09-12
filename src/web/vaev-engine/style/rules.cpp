module;

#include <karm-logger/logger.h>

export module Vaev.Engine:style.rules;

import Karm.Core;
import Karm.Gc;
import Karm.Ref;

import :css;
import :style.fonts;
import :style.media;
import :style.origin;
import :style.page;
import :style.props;
import :style.selector;
import :style.namespace_;
import :style.matcher;

using namespace Karm;

namespace Vaev::Style {

static bool DEBUG_RULE = false;

export struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
export struct StyleRule {
    Selector selector = TypeSelector::universal();
    Vec<StyleProp> props;
    Origin origin = Origin::AUTHOR;

    static StyleRule parse(Css::Sst const& sst, Origin origin, Namespace& ns) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        StyleRule res;

        // Parse the selector.
        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;
        auto maybeSelector = Selector::parse(prefixContent, ns);
        if (maybeSelector) {
            res.selector = maybeSelector.take();
        } else {
            logWarn("failed to parse selector: {}: {}", prefix->content, maybeSelector);
            res.selector = EmptySelector{};
        }

        // Parse the properties.
        for (auto const& item : sst.content) {
            if (item == Css::Sst::DECL) {
                auto prop = parseDeclaration<StyleProp>(item);
                if (prop)
                    res.props.pushBack(prop.take());
            } else {
                logWarnIf(DEBUG_RULE, "unexpected item in style rule: {}", item);
            }
        }

        res.origin = origin;
        return res;
    }

    Opt<Spec> match(Gc::Ref<Dom::Element> el) const {
        return matchSelector(selector, el);
    }

    void repr(Io::Emit& e) const {
        e("(style-rule");
        e.indent();
        e("\nselector: {}", selector);
        if (props) {
            e.newline();
            e("props: [");
            e.indentNewline();
            for (auto const& prop : props) {
                e("{}\n", prop);
            }
            e.deindent();
            e("]\n");
        }
        e.deindent();
        e(")");
    }
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
export struct ImportRule {
    Ref::Url url;

    static ImportRule parse(Css::Sst const&) {
        return {};
    }

    void repr(Io::Emit& e) const {
        e("(import-rule {})", url);
    }
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
export struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;

    static MediaRule parse(Css::Sst const& sst, Origin origin, Namespace& ns);

    bool match(Media const& m) const {
        return media.match(m);
    }

    void repr(Io::Emit& e) const;
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
export struct FontFaceRule {
    Vec<FontDesc> descs;

    static FontFaceRule parse(Css::Sst const& sst) {
        return {parseDeclarations<FontDesc>(sst, false)};
    }

    void repr(Io::Emit& e) const {
        e("(font-face-rule {})", descs);
    }
};

// https://drafts.csswg.org/css-namespaces/#syntax
export struct NamespaceRule {
    Opt<Symbol> prefix;
    Symbol url;

    static NamespaceRule parse(Css::Sst const& sst, Namespace& ns) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;

        eatWhitespace(prefixContent);
        Opt<Symbol> maybePrefix;
        if (*prefixContent == Css::Token::IDENT) {
            maybePrefix = Symbol::from(prefixContent->token.data);
            prefixContent.next();
        }

        eatWhitespace(prefixContent);
        auto maybeUrl = parseValue<String>(prefixContent);
        if (not maybeUrl) {
            maybeUrl = parseUrlIntoString(prefixContent);
        }
        if (not maybeUrl) {
            logWarn("expected namespace URI, got: {}", prefixContent);
            return {NONE, Symbol::EMPTY};
        }

        // Store the namespace.
        if (maybePrefix)
            ns.prefixes.put(maybePrefix.unwrap(), Symbol::from(maybeUrl.unwrap()));
        else
            ns.default_ = Symbol::from(maybeUrl.unwrap());

        return {maybePrefix, Symbol::from(maybeUrl.take())};
    }

    void repr(Io::Emit& e) const {
        e("(namespace-rule {} {})", prefix, url);
    }
};

export struct PageRule {
    Vec<PageSelector> selectors;
    Vec<StyleProp> props;
    Vec<PageAreaRule> areas;

    static PageRule parse(Css::Sst const& sst) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        PageRule res;

        // Parse the selector
        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;
        res.selectors = PageSelector::parseList(prefixContent);

        // Parse the properties.
        for (auto const& item : sst.content) {
            if (item == Css::Sst::DECL) {
                auto prop = parseDeclaration<StyleProp>(item);
                if (prop)
                    res.props.pushBack(prop.take());
            } else if (item == Css::Sst::RULE and
                       item.token == Css::Token::AT_KEYWORD) {
                auto rule = PageAreaRule::parse(item);
                if (rule)
                    res.areas.pushBack(*rule);
            } else {
                logWarnIf(DEBUG_RULE, "unexpected item in style rule: {}", item);
            }
        }

        return res;
    }

    bool match(Page const& page) const {
        if (selectors.len() == 0)
            return true;

        for (auto& s : selectors) {
            if (s.match(page))
                return true;
        }

        return false;
    }

    void apply(PageSpecifiedValues& c) const {
        for (auto const& prop : props) {
            prop.apply(*c.style, *c.style);
        }

        for (auto const& area : areas) {
            auto& areaPseudoElement = c.area(area.area);
            area.apply(*areaPseudoElement.specifiedValues());
        }
    }

    void repr(Io::Emit& e) const {
        e("(page-rule\nselectors: {}\nprops: {}\nmargins: {})", selectors, props, areas);
    }
};

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule,
    NamespaceRule,
    PageRule>;

export struct Rule : _Rule {
    using _Rule::_Rule;

    static Rule parse(Css::Sst const& sst, Origin origin, Namespace& ns) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        auto tok = sst.token;
        if (tok.data == "@media")
            return MediaRule::parse(sst, origin, ns);
        else if (tok.data == "@import")
            return ImportRule::parse(sst);
        else if (tok.data == "@font-face")
            return FontFaceRule::parse(sst);
        else if (tok.data == "@page")
            return PageRule::parse(sst);
        else if (tok.data == "@supports") {
            logWarn("cannot parse '@supports' at-rule");
            return StyleRule{};
        } else if (tok.data == "@namespace") {
            return NamespaceRule::parse(sst, ns);
        } else
            return StyleRule::parse(sst, origin, ns);
    }

    void repr(Io::Emit& e) const {
        visit([&](auto const& r) {
            e("{}", r);
        });
    }
};

MediaRule MediaRule::parse(Css::Sst const& sst, Origin origin, Namespace& ns) {
    if (sst != Css::Sst::RULE)
        panic("expected rule");

    if (sst.prefix != Css::Sst::LIST)
        panic("expected list");

    MediaRule res;

    // Parse the media query.
    auto& prefix = sst.prefix.unwrap();
    Cursor<Css::Sst> prefixContent = prefix->content;
    res.media = parseMediaQuery(prefixContent);

    // Parse the rules.
    for (auto const& item : sst.content) {
        if (item == Css::Sst::RULE) {
            res.rules.pushBack(Rule::parse(item, origin, ns));
        } else {
            logWarn("unexpected item in media rule: {}", item.type);
        }
    }

    return res;
}

void MediaRule::repr(Io::Emit& e) const {
    e("(media-rule");
    e.indent();
    e("\nmedia: {}", media);
    if (rules) {
        e.newline();
        e("rules: [");
        e.indentNewline();
        for (auto const& rule : rules) {
            e("{}\n", rule);
        }
        e.deindent();
        e("]\n");
    }
}

using MatchingRules = Vec<Tuple<Cursor<StyleRule>, Spec>>;

// Used to speed up the lookup of style rules by using lookup tables.
// This is useful for rules described by:
// - Simple selectors other than class selectors
// - OR or AND infixes that contain lookupable selectors
// - Complex selectors where the right-hand side is a lookupable selector
// Currently, only a subset of lookupable simple selectors are implemented:
//  - Attr selectors with non-qualified namespaces could also be lookupable selectors.
//  - All type selectors could be lookupable selectors.
//  - :is(), :where() could be lookuable selectors.
struct StyleRuleLookup {
    using RuleWithOrder = Tuple<usize, Cursor<StyleRule>>;

    usize _ruleCount = 0;

    Map<Str, Vec<RuleWithOrder>> _idRules;
    Map<Str, Vec<RuleWithOrder>> _classRules;
    Map<Symbol, Vec<RuleWithOrder>> _typeNameRules;
    Map<Symbol, Vec<RuleWithOrder>> _attrPresentRules;
    Map<Tuple<Symbol, Str>, Vec<RuleWithOrder>> _attrExactValueRules;

    Vec<RuleWithOrder> _nonLookupRules;

    Map<usize, usize> _ruleIdToNeededCount;

    void _add(Cursor<StyleRule> rule, usize ruleId, Selector const& selector) {
        selector.visit(Visitor{
            [&](TypeSelector const& s) {
                auto const& qualifiedNameSelector = s.qualifiedName;

                if (not isLookupEquivalentToMatch(qualifiedNameSelector)) {
                    _nonLookupRules.pushBack({ruleId, rule});
                    return;
                }

                _typeNameRules.getOrDefault(qualifiedNameSelector.name.unwrap()).pushBack({ruleId, rule});
            },
            [&](IdSelector const& s) {
                _idRules.getOrDefault(s.id.str()).pushBack({ruleId, rule});
            },
            [&](ClassSelector const& s) {
                _classRules.getOrDefault(s.class_).pushBack({ruleId, rule});
            },
            [&](AttributeSelector const& s) {
                if (not isLookupEquivalentToMatch(s)) {
                    _nonLookupRules.pushBack({ruleId, rule});
                    return;
                }

                auto const& name = *s.qualifiedName.name;

                if (s.match == AttributeSelector::Match::PRESENT) {
                    _attrPresentRules.getOrDefault(name).pushBack({ruleId, rule});
                } else if (s.match == AttributeSelector::Match::EXACT) {
                    _attrExactValueRules.getOrDefault({name, s.value}).pushBack({ruleId, rule});
                }
            },
            [&](Infix const& s) {
                if (isLookupEquivalentToMatch(s.rhs) or s.rhs->is<Nfix>()) {
                    _add(rule, ruleId, s.rhs);
                } else {
                    _nonLookupRules.pushBack({ruleId, rule});
                }
            },
            [&](Nfix const& s) {
                if (s.type == Nfix::AND) {
                    // NOTE: We could remove the lookupable selectors from the nfix since they are
                    // already handled by the lookup phase. However, computing specificy should be done
                    // before removing said selectors.
                    usize conditionsCount = 0;
                    for (auto const& inner : s.inners) {
                        if (isLookupEquivalentToMatch(inner)) {
                            conditionsCount++;
                            _add(rule, ruleId, inner);
                        }
                    }

                    if (conditionsCount == 0) {
                        _nonLookupRules.pushBack({ruleId, rule});
                    } else {
                        _ruleIdToNeededCount.put(ruleId, conditionsCount);
                    }
                } else if (s.type == Nfix::OR) {
                    bool hasNonLookupable = false;
                    for (auto const& inner : s.inners) {
                        if (isLookupEquivalentToMatch(inner)) {
                            _add(rule, ruleId, inner);
                        } else {
                            hasNonLookupable = true;
                        }
                    }
                    if (hasNonLookupable)
                        _nonLookupRules.pushBack({ruleId, rule});
                } else {
                    _nonLookupRules.pushBack({ruleId, rule});
                }
            },
            [&](auto const&) {
                _nonLookupRules.pushBack({ruleId, rule});
            },
        });
    }

    void add(StyleRule const& rule) {
        _ruleCount++;
        _add(&rule, _ruleCount, rule.selector);
    }

    static bool isLookupEquivalentToMatch(AttributeSelector const& selector) {
        if (selector.match != AttributeSelector::Match::PRESENT and
            selector.match != AttributeSelector::Match::EXACT)
            return false;

        return selector.qualifiedName.ns != NONE;
    }

    static bool isLookupEquivalentToMatch(TypeSelector const& selector) {
        return selector.qualifiedName.ns == NONE &&
               selector.qualifiedName.name != NONE;
    }

    static bool isLookupEquivalentToMatch(Selector const& selector) {
        if (auto s = selector.is<AttributeSelector>())
            return isLookupEquivalentToMatch(*s);

        if (auto s = selector.is<TypeSelector>())
            return isLookupEquivalentToMatch(*s);

        return selector.is<IdSelector>() or selector.is<ClassSelector>();
    }

    Vec<Cursor<RuleWithOrder>> _cursors;

    void _collectMatchedRulesCursors(Gc::Ref<Dom::Element> el) {
        _cursors.clear();

        auto considerCursorIfPresent = [](auto& lookup, auto const& key, Vec<Cursor<RuleWithOrder>>& _cursors) {
            auto rules = lookup.access(key);
            if (rules)
                _cursors.pushBack({rules->buf(), rules->len()});
        };

        for (auto const& class_ : el->classList._tokens) {
            considerCursorIfPresent(_classRules, class_.str(), _cursors);
        }

        if (auto id = el->id()) {
            considerCursorIfPresent(_idRules, *id, _cursors);
        }

        considerCursorIfPresent(_typeNameRules, el->qualifiedName.name, _cursors);

        for (auto const& [name, value] : el->attributes.iter()) {
            auto const& attrName = name.name;
            auto key = Tuple<Symbol, Str>{attrName, value->value.str()};

            considerCursorIfPresent(_attrPresentRules, attrName, _cursors);
            considerCursorIfPresent(_attrExactValueRules, key, _cursors);
        }

        if (_nonLookupRules.len())
            _cursors.pushBack({_nonLookupRules.buf(), _nonLookupRules.len()});
    }

    MatchingRules _matchingRules;

    void _evalStyleRule(StyleRule const& rule, Gc::Ref<Dom::Element> el) {
        if (auto specificity = rule.match(el))
            _matchingRules.pushBack({&rule, specificity.unwrap()});
    }

    bool _maybeDeferRuleEvaluation(RuleWithOrder const& RuleWithOrder, usize countMatchesWithCurrentRule) {
        auto const& [ruleId, styleRule] = RuleWithOrder;

        if (isLookupEquivalentToMatch(styleRule->selector)) {
            _matchingRules.pushBack({styleRule, spec(styleRule->selector)});
            return true;
        }

        auto nfix = styleRule->selector.is<Nfix>();

        if (not nfix or (nfix->type != Nfix::AND and nfix->type != Nfix::OR))
            return false;

        if (nfix->type == Nfix::OR) {
            // Deffering the evaluation to after we know how many times this rule was matched.
            return true;
        }

        auto neededCount = _ruleIdToNeededCount.tryGet(ruleId);

        if (not neededCount) {
            // This selector doesnt not have a needed count, meaning that is has no lookupable selectors.
            return false;
        }

        if (countMatchesWithCurrentRule != *neededCount) {
            // We still expect more internal lookupable selectors to be matched for this AND Nfix
            return true;
        }

        if (nfix->inners.len() != countMatchesWithCurrentRule) {
            // We matched all lookupable selectors as a "pre-condition" to evaluate the rule,
            // but we need now to evaluate the whole rule since it has non-lookupable selectors.
            return false;
        }

        _matchingRules.pushBack({styleRule, spec(styleRule->selector)});
        return true;
    }

    void _mergeMatchedRules(Gc::Ref<Dom::Element> el) {
        usize countMatchesWithCurrentRule = 0;
        usize lastRuleId = 0;
        Cursor<StyleRule> lastStyleRule = nullptr;

        auto maybeFinalizeNfixOrRule = [&]() {
            if (not lastStyleRule)
                return;

            if (auto nfix = lastStyleRule->selector.is<Nfix>()) {
                if (nfix->type != Nfix::OR)
                    return;

                if (countMatchesWithCurrentRule == 1) {
                    _evalStyleRule(*lastStyleRule, el);
                } else {
                    // NOTE: If an element has 2 or more occourence of this rule in its list, we can assume
                    // the rule as matched, since at least one of the occourences is due to a lookupable selector,
                    // which is guaranteed to match.
                    _matchingRules.pushBack({lastStyleRule, spec(lastStyleRule->selector)});
                }
            }
        };

        while (_cursors.len() > 0) {
            usize bestCursorIdx = 0;
            for (usize i = 1; i < _cursors.len(); i++) {
                if (_cursors[i]->v0 < _cursors[bestCursorIdx]->v0) {
                    bestCursorIdx = i;
                }
            }

            // NOTE: This is quite hot code and doing this check every time is not ideal,
            // but it was the only way found to allow defering the evaluation of OR infixes until
            // we know how many times this rule was matched.
            if (lastRuleId != _cursors[bestCursorIdx]->v0) {
                maybeFinalizeNfixOrRule();
                countMatchesWithCurrentRule = 1;
            } else {
                countMatchesWithCurrentRule++;
            }

            if (not _maybeDeferRuleEvaluation(*_cursors[bestCursorIdx], countMatchesWithCurrentRule))
                _evalStyleRule(*_cursors[bestCursorIdx]->v1, el);

            lastStyleRule = _cursors[bestCursorIdx]->v1;
            lastRuleId = _cursors[bestCursorIdx]->v0;

            _cursors[bestCursorIdx].next();
            if (_cursors[bestCursorIdx].ended()) {
                std::swap(_cursors[bestCursorIdx], last(_cursors));
                _cursors.popBack();
            }
        }

        maybeFinalizeNfixOrRule();
    }

    MatchingRules buildMatchingRules(Gc::Ref<Dom::Element> el) {
        _cursors.clear();
        _matchingRules.clear();

        _collectMatchedRulesCursors(el);
        _mergeMatchedRules(el);

        return _matchingRules;
    }
};

} // namespace Vaev::Style
