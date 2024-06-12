#pragma once

#include <web-style/stylesheet.h>

#include "sst.h"

namespace Web::Css {

static Res<Style::Selector> getSelectorElement(Vec<Sst> const &prefix, usize &i) {
    logDebug("new selector found {}", prefix[i].token);

    switch (prefix[i].token.type) {
    case Token::HASH:
        return Ok(Style::IdSelector{next(prefix[i].token.data, 1)});
    case Token::IDENT:
        return Ok(Style::TypeSelector{TagName::make(prefix[i].token.data, Web::HTML)});
    case Token::DELIM:
        if (prefix[i].token.data == ".") {
            if (i >= prefix.len()) {
                return Error();
            }
            i++;
            logDebug("new selector found {}", prefix[i].token);
            return Ok(Style::ClassSelector{prefix[i].token.data});
        } else if (prefix[i].token.data == "*") {
            return Ok(Style::UniversalSelector{});
        }

    default:
        return Ok(Style::ClassSelector{prefix[i].token.data});
    }
}

static Res<Style::Selector> composeSelectors(Style::Selector sel1, Style::Selector sel2) {
    logDebug("composing {} and {}", sel1, sel2);
    if (!sel1.is<Style::Infix>()) {
        return Ok(Style::Selector::and_(Vec<Style::Selector>{sel1, sel2}));
    }

    return Ok(sel1);
}

static Res<Style::Selector> parseSelector(auto prefix) {
    bool ignoreWhitespace = true;
    usize i = 0;
    Style::Selector currentSelector = try$(getSelectorElement(prefix, i));
    i++;
    while (i < prefix.len()) {
        if (prefix[i].type == Sst::TOKEN) {
            switch (prefix[i].token.type) {
            case Token::WHITESPACE:
                if (ignoreWhitespace) {
                    break;
                }
                break;
            default:
                currentSelector = try$(composeSelectors(currentSelector, try$(getSelectorElement(prefix, i))));
                break;
            }
        }

        i++;
    }
    return Ok(currentSelector);
}

Res<Style::Rule> getRuleObject(auto prefix) {
    switch (prefix[0].type) {
    case Sst::RULE:
    case Sst::FUNC:
    case Sst::DECL:
    case Sst::LIST:
    case Sst::BLOCK:
        return Error();
    case Sst::TOKEN:
        auto tok = prefix[0].token;
        switch (tok.type) {
        case Token::AT_KEYWORD: {
            if (tok.data == "@font-face") {
                Style::FontFaceRule parsed;
                return Ok(parsed);
            } else if (tok.data == "@media") {
                auto query = Style::MediaQuery::combineOr(
                    Style::TypeFeature{MediaType::SCREEN},
                    Style::WidthFeature::min(Px{1920})
                );
                Style::MediaRule parsed(query);
                return Ok(parsed);
            }
        }
        default: {
            Style::StyleRule parsed = Style::StyleRule(try$(parseSelector(prefix)));
            // parsed.selector = try$(parseSelector());
            return Ok(parsed);
        }
        }
    }
}

static Res<Style::Rule> parseQualifiedRule(Sst rule) {
    auto &prefix = rule.prefix.unwrap()->content;

    auto parsed = try$(getRuleObject(prefix));

    auto block = rule.content[0].content;
    bool parsingContent = false;
    for (usize i = 0; i < block.len(); i++) {
        switch (block[i].type) {
        case Sst::RULE:
        case Sst::DECL:
        case Sst::LIST:
        case Sst::BLOCK:
            break;
        case Sst::TOKEN:
            if (not parsingContent) {
                if (block[i].token.type != Token::WHITESPACE) {
                    if (block[i].token.type != Token::COLON) {
                        // ignore
                    } else {
                        parsingContent = true;
                    }
                }
            } else {
                if (block[i].token.type != Token::SEMICOLON) {
                    // ignore
                } else {
                    parsingContent = false;
                }
            }
            break;
        case Sst::FUNC:
            break;
        }
    }
    return Ok(parsed);
}

// No spec, we take the SST we built and convert it to a usable list of rules
static inline Vec<Style::Rule> parseSST(Sst sst) {
    Vec<Style::Rule> rules;
    for (usize i = 0; i < sst.content.len(); i++) {
        switch (sst.content[i].type) {

        case Sst::RULE:
            rules.pushBack(parseQualifiedRule(sst.content[i]).unwrap());
            break;
        case Sst::FUNC:
        case Sst::DECL:
        case Sst::LIST:
        case Sst::TOKEN:
        case Sst::BLOCK:
            break;
        }
    }
    return rules;
}

} // namespace Web::Css
