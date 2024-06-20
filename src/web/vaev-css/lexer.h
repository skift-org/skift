#pragma once

#include <karm-base/ctype.h>
#include <karm-base/func.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/emit.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>

namespace Vaev::Css {

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

    void repr(Io::Emit &e) const;

    bool operator==(Type type) const {
        return this->type == type;
    }

    bool operator==(Token const &other) const {
        return type == other.type && data == other.data;
    }
};

struct Lexer {
    Io::SScan _scan;
    Token _curr;

    Lexer(Str text) : _scan(text) {
        _curr = _next(_scan);
    }

    Lexer(Io::SScan const &scan)
        : _scan(scan) {
        _curr = _next(_scan);
    }

    Token peek() const {
        return _curr;
    }

    Token _nextIdent(Io::SScan &) const;

    Token _next(Io::SScan &) const;

    Token next() {
        auto res = _curr;
        _curr = _next(_scan);
        return res;
    }

    bool ended() const {
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

} // namespace Vaev::Css

#undef FOREACH_TOKEN
