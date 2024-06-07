#pragma once

#include <karm-base/ctype.h>
#include <karm-base/func.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>

namespace Web::Css {

#define FOREACH_TOKEN(TOKEN)                     \
    TOKEN(NIL)                  /* no a token */ \
    TOKEN(IDENT)                /* foo */        \
    TOKEN(FUNCTION)             /* calc( */      \
    TOKEN(AT_KEYWORD)           /* @import */    \
    TOKEN(HASH)                 /* #foo */       \
    TOKEN(STRING)               /* "foo" */      \
    TOKEN(BAD_STRING)           /* "foo */       \
    TOKEN(URL)                  /* url(foo) */   \
    TOKEN(BAD_URL)              /* url(foo */    \
    TOKEN(DELIM)                /* !, +, - */    \
    TOKEN(NUMBER)               /* 123 */        \
    TOKEN(PERCENTAGE)           /* 123% */       \
    TOKEN(DIMENSION)            /* 123px */      \
    TOKEN(WHITESPACE)           /* ' ' */        \
    TOKEN(CDO)                  /* <!-- */       \
    TOKEN(CDC)                  /* --> */        \
    TOKEN(COLON)                /* : */          \
    TOKEN(SEMICOLON)            /* ; */          \
    TOKEN(COMMA)                /* , */          \
    TOKEN(LEFT_CURLY_BRACKET)   /* { */          \
    TOKEN(RIGHT_CURLY_BRACKET)  /* } */          \
    TOKEN(LEFT_SQUARE_BRACKET)  /* [ */          \
    TOKEN(RIGHT_SQUARE_BRACKET) /* ] */          \
    TOKEN(LEFT_PARENTHESIS)     /* ( */          \
    TOKEN(RIGHT_PARENTHESIS)    /* ) */          \
    TOKEN(COMMENT)              /* */            \
    TOKEN(END_OF_FILE)          /* EOF */        \
    TOKEN(OTHER)                /* anything else */

struct Token {
    enum struct Type {
#define ITER(NAME) NAME,
        FOREACH_TOKEN(ITER)
#undef ITER
    };

    using enum Type;

    Type type;
    Str data;

    Token() : type(NIL) {}

    Token(Type type, Str data = "")
        : type(type), data(data) {}

    explicit operator bool() const {
        return type != NIL;
    }
};

struct Lexer {
    Io::SScan &_scan;
    Token _curr{};

    Lexer(Io::SScan &scan)
        : _scan(scan) {
        _curr = _next();
    }

    Token curr() {
        return _curr;
    }

    Token _next();

    Token next() {
        _curr = _next();
        return _curr;
    }

    bool ended() {
        return _scan.ended();
    }
};

static inline Str toStr(Token::Type type) {
    switch (type) {
#define ITER(NAME)    \
    case Token::NAME: \
        return #NAME;
        FOREACH_TOKEN(ITER)
#undef ITER
    default:
        panic("invalid token type");
    }
}

} // namespace Web::Css

template <>
struct Karm::Io::Formatter<Web::Css::Token> {
    Res<usize> format(Io::TextWriter &writer, Web::Css::Token const &val) {
        if (not val)
            return Io::format(writer, "nil");

        return Io::format(
            writer,
            "({} {#})",
            try$(Io::toParamCase(Web::Css::toStr(val.type))),
            val.data
        );
    }
};
