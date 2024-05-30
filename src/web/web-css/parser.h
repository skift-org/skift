#pragma once

#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/file.h>
#include <web-cssom/stylesheet.h>

#include "builder.h"
#include "lexer.h"

namespace Web::Css {

Res<Sst> _consumeComponentValue(Io::SScan &s, Token token);

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Res<Sst> _consumeBlock(Io::SScan &s, Token::Type term) {
    Sst block = Sst::BLOCK;

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
Res<Sst> _consumeFunc(Io::SScan &s, Token token) {
    Sst fn = Sst::FUNC;
    Sst ast = Sst::TOKEN;
    ast.token = token;
    fn.prefix = Box{ast};

    while (true) {
        auto token = nextToken(s).unwrap();

        switch (token.type) {
        case Token::RIGHT_PARENTHESIS:
            return Ok(std::move(fn));
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            return Ok(std::move(fn));
        default:
            fn.content.emplaceBack(try$(_consumeComponentValue(s, token)));
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
Res<Sst> _consumeComponentValue(Io::SScan &s, Token token) {
    switch (token.type) {
    case Token::LEFT_SQUARE_BRACKET:
        return _consumeBlock(s, Token::RIGHT_SQUARE_BRACKET);

    case Token::LEFT_CURLY_BRACKET:
        return _consumeBlock(s, Token::RIGHT_CURLY_BRACKET);

    case Token::LEFT_PARENTHESIS:
        return _consumeBlock(s, Token::RIGHT_PARENTHESIS);

    case Token::FUNCTION:
        return _consumeFunc(s, token);

    default:
        Sst tok = Sst::TOKEN;
        tok.token = token;
        return Ok(std::move(tok));
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Res<Sst> _consumeQualifiedRule(Io::SScan &s, Token token) {
    Sst ast{Sst::QUALIFIED_RULE};
    Sst pre{Sst::LIST};

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
Res<Sst> _consumeRuleList(Str str) {
    auto s = Io::SScan(str);
    Sst ast = Sst::LIST;

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
    Sst ast = try$(_consumeRuleList(buf));
    logDebug("AST : {#}", ast);
    stylesheet.cssRules = parseSST(ast);
    logDebug("RULES : {#}", stylesheet.cssRules);
    return Ok();
}

static inline Str toStr(Sst::_Type type) {
    switch (type) {
#define ITER(NAME)  \
    case Sst::NAME: \
        return #NAME;
        FOREACH_SST(ITER)
#undef ITER
    default:
        panic("invalid ast type");
    }
}

// NOSPEC, used to try the lexer independently
Res<> testLexer(Str source) {
    auto file = try$(Sys::File::open(Mime::parseUrlOrPath(source).unwrap()));
    auto buf = try$(Io::readAllUtf8(file));
    auto s = Io::SScan(buf);
    while (true) {
        auto token = try$(nextToken(s));
        logDebug("token found {#}", token);
        switch (token.type) {
        case Token::END_OF_FILE:
            return Ok();
        case Token::OTHER:
            return Error();
        default:
            break;
        }
    }
    return Error();
}

} // namespace Web::Css

Reflectable$(Web::Css::Sst, type, token, prefix, content);

template <>
struct Karm::Io::Formatter<Web::Css::Sst::_Type> {
    Res<usize> format(Io::TextWriter &writer, Web::Css::Sst::_Type val) {
        return (writer.writeStr(try$(Io::toParamCase(toStr(val)))));
    }
};
