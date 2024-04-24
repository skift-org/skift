#pragma once

#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-cssom/stylesheet.h>

#include "lexer.h"

namespace Web::Css {

struct Ast;

using Block = Vec<Box<Ast>>;

struct Ast {
    enum struct Type {
        QUALIFIED_RULE,
        FUNC,
        DECL,
        LIST,
        BLOCK,
        TOKEN,
    };

    using enum Type;

    Type type;
    Opt<Token> token;
    Opt<Box<Ast>> pre;
    Block block;
};

Res<Ast> _consumeComponentValue(Io::SScan &s, Token token);

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Res<Ast> _consumeBlock(Io::SScan &s, Token::Type term) {
    Ast block;
    block.type = Ast::Type::BLOCK;

    while (true) {
        auto token = nextToken(s).unwrap();

        switch (token.type) {
        case Token::Type::END_OF_FILE:
            // this is a parse error
            return Ok(std::move(block));
        default:
            if (token.type == term) {
                return Ok(std::move(block));
            } else {
                block.block.emplaceBack(try$(_consumeComponentValue(s, token)));
            }
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-function
Res<Ast> _consumeFunc(Io::SScan &s) {
    Ast fn;
    fn.type = Ast::Type::FUNC;

    while (true) {
        auto token = nextToken(s).unwrap();

        switch (token.type) {
        case Token::RIGHT_PARENTHESIS:
            return Ok(std::move(fn));
        case Token::Type::END_OF_FILE:
            // this is a parse error
            return Ok(std::move(fn));
        default:
            fn.block.emplaceBack(try$(_consumeComponentValue(s, token)));
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
Res<Ast> _consumeComponentValue(Io::SScan &s, Token token) {
    switch (token.type) {
    case Token::Type::LEFT_SQUARE_BRACKET:
        return (_consumeBlock(s, Token::Type::RIGHT_SQUARE_BRACKET));

    case Token::Type::LEFT_CURLY_BRACKET:
        return (_consumeBlock(s, Token::Type::RIGHT_CURLY_BRACKET));

    case Token::Type::LEFT_PARENTHESIS:
        return (_consumeBlock(s, Token::Type::RIGHT_PARENTHESIS));

    case Token::Type::FUNCTION:
        return _consumeFunc(s);

    default:
        Ast tok;
        tok.type = Ast::TOKEN;
        tok.token = token;
        return Ok(std::move(tok));
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Res<Ast> _consumeQualifiedRule(Io::SScan &s, Token token) {
    Ast ast;
    ast.type = Ast::QUALIFIED_RULE;

    while (true) {
        switch (token.type) {
        case Token::Type::END_OF_FILE:
            return Error();
        case Token::Type::LEFT_CURLY_BRACKET:
            ast.block.emplaceBack(try$(_consumeBlock(s, Token::RIGHT_CURLY_BRACKET)));
            return Ok(ast);
        default:
            ast.pre.emplace(try$(_consumeComponentValue(s, token)));
            break;
        }
        token = nextToken(s).unwrap();
    }

    return Ok(ast);
}

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-rules
Res<Ast> _consumeRuleList(Str str) {
    auto s = Io::SScan(str);
    Ast ast;
    ast.type = Ast::LIST;

    while (true) {
        auto token = try$(nextToken(s));
        switch (token.type) {
        case Token::END_OF_FILE:
            return Ok(ast);
        case Token::WHITESPACE:
            break;
        default:
            ast.block.pushBack(try$(_consumeQualifiedRule(s, token)));
            break;
        }
    }
}

// No spec, we take the AST we built and convert it to a usable list of rules
Vec<CSSOM::CSSStyleRule> parseAST(Ast) {
    Vec<CSSOM::CSSStyleRule> rules;

    return rules;
}

// https://www.w3.org/TR/css-syntax-3/#parse-stylesheet
Res<> parseStylesheet(Str source) {
    CSSOM::StyleSheet stylesheet = CSSOM::StyleSheet(source);
    auto file = try$(Sys::File::open(Mime::parseUrlOrPath(source).unwrap()));
    auto buf = try$(Io::readAllUtf8(file));
    Ast ast = try$(_consumeRuleList(buf));
    stylesheet.cssRules = parseAST(ast);
    logDebug("AST : {}", ast);
    return Ok();
}

} // namespace Web::Css

Reflectable$(Web::Css::Ast, token, pre, block);