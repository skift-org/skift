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

#define FOREACH_TOKEN(TOKEN)                                         \
    TOKEN(NIL, nil)                                 /* no a token */ \
    TOKEN(IDENT, ident)                             /* foo */        \
    TOKEN(FUNCTION, function)                       /* calc( */      \
    TOKEN(AT_KEYWORD, atKeyword)                    /* @import */    \
    TOKEN(HASH, hash)                               /* #foo */       \
    TOKEN(STRING, string)                           /* "foo" */      \
    TOKEN(BAD_STRING, badString)                    /* "foo */       \
    TOKEN(URL, url)                                 /* url(foo) */   \
    TOKEN(BAD_URL, badUrl)                          /* url(foo */    \
    TOKEN(DELIM, delim)                             /* !, +, - */    \
    TOKEN(NUMBER, number)                           /* 123 */        \
    TOKEN(PERCENTAGE, percentage)                   /* 123% */       \
    TOKEN(DIMENSION, dimension)                     /* 123px */      \
    TOKEN(WHITESPACE, whitespace)                   /* ' ' */        \
    TOKEN(CDO, cdo)                                 /* <!-- */       \
    TOKEN(CDC, cdc)                                 /* --> */        \
    TOKEN(COLON, colon)                             /* : */          \
    TOKEN(SEMICOLON, semicolon)                     /* ; */          \
    TOKEN(COMMA, comma)                             /* , */          \
    TOKEN(LEFT_CURLY_BRACKET, leftCurlyBracket)     /* { */          \
    TOKEN(RIGHT_CURLY_BRACKET, rightCurlyBracket)   /* } */          \
    TOKEN(LEFT_SQUARE_BRACKET, leftSquareBracket)   /* [ */          \
    TOKEN(RIGHT_SQUARE_BRACKET, rightSquareBracket) /* ] */          \
    TOKEN(LEFT_PARENTHESIS, leftParenthesis)        /* ( */          \
    TOKEN(RIGHT_PARENTHESIS, rightParenthesis)      /* ) */          \
    TOKEN(COMMENT, comment)                         /* */            \
    TOKEN(END_OF_FILE, endOfFile)                   /* EOF */        \
    TOKEN(OTHER, other)                             /* anything else */

struct Token {
    enum struct Type {
#define ITER(NAME, ...) NAME,
        FOREACH_TOKEN(ITER)
#undef ITER

            _LEN,
    };

    using enum Type;

    Type type;
    String data;

#define ITER(ID, NAME) \
    static Token NAME(Str data = "") { return {ID, data}; }
    FOREACH_TOKEN(ITER)
#undef ITER

    Token() : type(NIL) {}

    Token(Type type, String data = ""s)
        : type(type), data(data) {}

    explicit operator bool() const {
        return type != NIL;
    }

    void repr(Io::Emit& e) const;

    bool operator==(Type type) const {
        return this->type == type;
    }

    bool operator==(Token const& other) const {
        return type == other.type and data == other.data;
    }
};

struct Lexer {
    Io::SScan _scan;
    Token _curr;

    Lexer(Str text) : _scan(text) {
        _curr = _next(_scan);
    }

    Lexer(Io::SScan const& scan)
        : _scan(scan) {
        _curr = _next(_scan);
    }

    Token peek() const {
        return _curr;
    }

    Token _nextIdent(Io::SScan&) const;

    Token _next(Io::SScan&) const;

    Token next() {
        auto res = _curr;
        _curr = _next(_scan);
        return res;
    }

    bool ended() const {
        return _scan.ended();
    }
};

} // namespace Vaev::Css

#undef FOREACH_TOKEN
