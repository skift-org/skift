#pragma once

#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-cssom/stylesheet.h>

#include "builder.h"
#include "lexer.h"

namespace Web::Css {
Res<Ast> _consumeComponentValue(Io::SScan &s, Token token);

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Res<Ast> _consumeBlock(Io::SScan &s, Token::Type term) {
    Ast block = Ast::BLOCK;

    while (true) {
        auto token = nextToken(s).unwrap();

        switch (token.type) {
        case Token::END_OF_FILE:
            // this is a parse error
            return Ok(block);
        default:
            if (token.type == term) {
                logDebug("closing the curent block");
                return Ok(block);
            } else {
                auto a = try$(_consumeComponentValue(s, token));
                logDebug("adding {#} to the curent block", a);
                block.content.emplaceBack(a);
            }
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-function
Res<Ast> _consumeFunc(Io::SScan &s) {
    Ast fn = Ast::FUNC;

    while (true) {
        auto token = nextToken(s).unwrap();

        switch (token.type) {
        case Token::RIGHT_PARENTHESIS:
            return Ok(std::move(fn));
        case Token::Type::END_OF_FILE:
            // this is a parse error
            return Ok(std::move(fn));
        default:
            fn.content.emplaceBack(try$(_consumeComponentValue(s, token)));
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
        Ast tok = Ast::TOKEN;
        tok.token = token;
        return Ok(std::move(tok));
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Res<Ast> _consumeQualifiedRule(Io::SScan &s, Token token) {
    Ast ast{Ast::QUALIFIED_RULE};
    Ast pre{Ast::LIST};

    while (true) {
        switch (token.type) {
        case Token::END_OF_FILE:
            return Error();
        case Token::LEFT_CURLY_BRACKET:
            logDebug("creating a block for curent rule");
            ast.prefix = Box{pre};
            ast.content.emplaceBack(try$(_consumeBlock(s, Token::RIGHT_CURLY_BRACKET)));
            return Ok(ast);
        default:
            auto a = try$(_consumeComponentValue(s, token));
            logDebug("adding a new component value to the prefix {#}", a);
            pre.content.pushBack(a);
            break;
        }
        token = nextToken(s).unwrap();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-rules
Res<Ast> _consumeRuleList(Str str) {
    auto s = Io::SScan(str);
    Ast ast = Ast::LIST;

    while (true) {
        auto token = try$(nextToken(s));
        switch (token.type) {
        case Token::END_OF_FILE:
            return Ok(ast);
        case Token::COMMENT:
        case Token::WHITESPACE:
            break;
        default:
            ast.content.pushBack(try$(_consumeQualifiedRule(s, token)));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#parse-stylesheet
Res<> parseStylesheet(Str source) {
    CSSOM::StyleSheet stylesheet = CSSOM::StyleSheet(source);
    auto file = try$(Sys::File::open(Mime::parseUrlOrPath(source).unwrap()));
    auto buf = try$(Io::readAllUtf8(file));
    Ast ast = try$(_consumeRuleList(buf));
    logDebug("AST : {#}", ast);
    stylesheet.cssRules = parseAST(ast);
    logDebug("RULES : {#}", stylesheet.cssRules);
    return Ok();
}

static inline Str toStr(Ast::_Type type) {
    switch (type) {
#define ITER(NAME)  \
    case Ast::NAME: \
        return #NAME;
        FOREACH_AST(ITER)
#undef ITER
    default:
        panic("invalid ast type");
    }
}

} // namespace Web::Css

Reflectable$(Web::Css::Ast, type, token, prefix, content);

template <>
struct Karm::Io::Formatter<Web::Css::Ast::_Type> {
    Res<usize> format(Io::TextWriter &writer, Web::Css::Ast::_Type val) {
        return (writer.writeStr(try$(Io::toParamCase(toStr(val)))));
    }
};
