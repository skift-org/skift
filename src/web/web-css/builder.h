#pragma once

#include <web-cssom/stylesheet.h>

#include "ast.h"

namespace Web::Css {

static CSSOM::CSSStyleRule parseQualifiedRule(Ast rule) {
    CSSOM::CSSStyleRule parsed;

    auto &prefix = rule.prefix.unwrap()->content;
    for (usize i = 0; i < prefix.len(); i++) {
        switch (prefix[i].type) {
        case Ast::QUALIFIED_RULE:
        case Ast::FUNC:
        case Ast::DECL:
        case Ast::LIST:
        case Ast::BLOCK:
            break;
        case Ast::TOKEN:
            parsed.selector.add(prefix[i].token.unwrap());
            break;
        }
    }

    auto block = rule.content[0].content;
    bool parsingContent = false;
    for (usize i = 0; i < block.len(); i++) {
        switch (block[i].type) {
        case Ast::QUALIFIED_RULE:
        case Ast::FUNC:
        case Ast::DECL:
        case Ast::LIST:
        case Ast::BLOCK:
            break;
        case Ast::TOKEN:
            if (!parsingContent) {
                if (block[i].token->type != Token::WHITESPACE) {
                    if (block[i].token->type != Token::COLON) {
                        parsed.declarations.add(CSSOM::CSSStyleDeclaration(block[i].token.unwrap()));
                    } else {
                        parsingContent = true;
                    }
                }
            } else {
                if (block[i].token->type != Token::SEMICOLON) {
                    last(parsed.declarations).value.add(block[i].token.unwrap());
                } else {
                    parsingContent = false;
                }
            }
            break;
        }
    }
    return parsed;
}

// No spec, we take the AST we built and convert it to a usable list of rules
Vec<CSSOM::CSSStyleRule> parseAST(Ast ast) {
    Vec<CSSOM::CSSStyleRule> rules;
    for (usize i = 0; i < ast.content.len(); i++) {
        switch (ast.content[i].type) {

        case Ast::_Type::QUALIFIED_RULE:
            rules.add(parseQualifiedRule(ast.content[i]));
            break;
        case Ast::_Type::FUNC:
        case Ast::_Type::DECL:
        case Ast::_Type::LIST:
        case Ast::_Type::TOKEN:
        case Ast::_Type::BLOCK:
            break;
        }
    }
    return rules;
}

} // namespace Web::Css