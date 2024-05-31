#pragma once

#include <web-cssom/stylesheet.h>

#include "sst.h"

namespace Web::Css {

static Res<Select::Selector> parseSelector(auto prefix) {
    bool ignoreWhitespace = true;
    // bool composite = false;

    for (usize i = 1; i < prefix.len(); i++) {
        switch (prefix[i].type) {
        case Sst::QUALIFIED_RULE:
        case Sst::FUNC:
        case Sst::DECL:
        case Sst::LIST:
        case Sst::BLOCK:
            // error ?
            break;
        case Sst::TOKEN:
            switch (prefix[i].token.unwrap().type) {
            case Token::WHITESPACE:
                if (ignoreWhitespace) {
                    break;
                }
                break;
            case Token::IDENT:
                //
                break;
            default:
                break;
            }
            Sst content = Sst(Sst::TOKEN);
            content.token = prefix[i].token.unwrap();
            // selector.add(content);
            break;
        }
    }
    Sst content = Sst(Sst::TOKEN);
    content.token = prefix[0].token.unwrap();
    Select::Selector selector = Select::ClassSelector{"foo"s};
    // selector.add(content);
    return Ok(selector);
}

Res<CSSOM::CSSRule> getRuleObject(auto prefix) {
    switch (prefix[0].type) {
    case Sst::QUALIFIED_RULE:
    case Sst::FUNC:
    case Sst::DECL:
    case Sst::LIST:
    case Sst::BLOCK:
        return Error();
    case Sst::TOKEN:
        auto tok = prefix[0].token.unwrap();
        switch (tok.type) {
        case Token::HASH: {
            if (tok.data == "@font-face") {
                CSSOM::CSSFontFaceRule parsed;
                return Ok(parsed);
            } else if (tok.data == "@suports") {
                CSSOM::CSSSupportsRule parsed;
                return Ok(parsed);
            } else if (tok.data == "@media") {
                CSSOM::CSSSupportsRule parsed;
                return Ok(parsed);
            }
        }
        default: {
            CSSOM::CSSStyleRule parsed = CSSOM::CSSStyleRule(try$(parseSelector(prefix)));
            // parsed.selector = try$(parseSelector());
            return Ok(parsed);
        }
        }
    }
}

static Res<CSSOM::CSSRule> parseQualifiedRule(Sst rule) {
    auto &prefix = rule.prefix.unwrap()->content;

    auto parsed = try$(getRuleObject(prefix));

    auto block = rule.content[0].content;
    bool parsingContent = false;
    for (usize i = 0; i < block.len(); i++) {
        switch (block[i].type) {
        case Sst::QUALIFIED_RULE:
        case Sst::DECL:
        case Sst::LIST:
        case Sst::BLOCK:
            break;
        case Sst::TOKEN:
            if (not parsingContent) {
                if (block[i].token->type != Token::WHITESPACE) {
                    if (block[i].token->type != Token::COLON) {
                        parsed.visit([&](auto &p) {
                            p.declarations.add(CSSOM::CSSStyleDeclaration(block[i].token.unwrap()));
                        });
                    } else {
                        parsingContent = true;
                    }
                }
            } else {
                if (block[i].token->type != Token::SEMICOLON) {
                    parsed.visit([&](auto &p) {
                        last(p.declarations).value.add(block[i].token.unwrap());
                    });
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
Vec<CSSOM::CSSRule> parseSST(Sst sst) {
    Vec<CSSOM::CSSRule> rules;
    for (usize i = 0; i < sst.content.len(); i++) {
        switch (sst.content[i].type) {

        case Sst::_Type::QUALIFIED_RULE:
            rules.add(parseQualifiedRule(sst.content[i]).unwrap());
            break;
        case Sst::_Type::FUNC:
        case Sst::_Type::DECL:
        case Sst::_Type::LIST:
        case Sst::_Type::TOKEN:
        case Sst::_Type::BLOCK:
            break;
        }
    }
    return rules;
}

} // namespace Web::Css
