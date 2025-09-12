module;

#include <karm-logger/logger.h>

export module Vaev.Engine:css.lexer;

import Karm.Core;

using namespace Karm;

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

export struct Token {
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

    void repr(Io::Emit& e) const {
        if (not *this) {
            e("nil");
            return;
        }

        e("({} {#})", type, data);
    }

    bool operator==(Type type) const {
        return this->type == type;
    }

    bool operator==(Token const& other) const {
        return type == other.type and data == other.data;
    }
};

static auto const RE_BRACKET_OPEN = Re::single('{');
static auto const RE_BRACKET_CLOSE = Re::single('}');
static auto const RE_PARENTHESIS_OPEN = Re::single('(');
static auto const RE_PARENTHESIS_CLOSE = Re::single(')');
static auto const RE_SQUARE_BRACKET_OPEN = Re::single('[');
[[maybe_unused]] static auto const RE_SQUARE_BRACKET_CLOSE = Re::single(']');
static auto const RE_SEMICOLON = Re::single(';');
static auto const RE_COLON = Re::single(':');
static auto const RE_COMMA = Re::single(',');
static auto const RE_QUOTES = Re::single('"', '\'');

// NOSPEC: In the spec the fallback is delim
// since DELIM has a semantic meaning and could catch a really unexpected content
// we chose to use OTHER as a fallback and delim is a determined set of characters
static auto const RE_DELIM = '.'_re | '+'_re | '-'_re | '#'_re | '~'_re | '!'_re | '*'_re | '='_re | '<'_re | '>'_re | '^'_re | '/'_re;

static auto const RE_NEWLINE = Re::either(Re::single('\n', '\r', '\f'), Re::word("\r\n"));
static auto const RE_ASCII = Re::range(0x00, 0x7f);
static auto const RE_WHITESPACE = Re::either(Re::space(), Re::single('\t'), RE_NEWLINE);
static auto const RE_WHITESPACE_TOKEN = Re::oneOrMore(Re::either(Re::space(), Re::single('\t'), RE_NEWLINE));

static auto const RE_ESCAPE = Re::chain(
    '\\'_re,
    Re::either(
        Re::oneOrMore(
            Re::negate(
                Re::either(
                    Re::xdigit(),
                    RE_NEWLINE
                )
            )
        ),
        Re::chain(
            Re::xdigit(),
            Re::atMost(5, Re::xdigit()),
            Re::zeroOrOne(RE_WHITESPACE)
        )
    )
);

[[maybe_unused]] static auto const RE_UNICODE = Re::chain(Re::single('U', 'u'), Re::oneOrMore(Re::xdigit()));

static auto const RE_IDENTIFIER = Re::chain(
    Re::either(
        Re::word("--"),
        Re::chain(
            Re::zeroOrOne(
                Re::single('-')
            ),
            Re::either(
                RE_ESCAPE,
                Re::either(
                    Re::alpha(),
                    Re::single('_'),
                    Re::negate(RE_ASCII)
                )
            )
        )
    ),
    Re::zeroOrOne(
        Re::oneOrMore(
            Re::either(
                RE_ESCAPE,
                Re::either(
                    Re::alnum(),
                    Re::single('_', '-'),
                    Re::negate(RE_ASCII)
                )
            )
        )
    )
);

static auto const RE_DIGIT = Re::oneOrMore(Re::digit());
static auto const RE_OPERATOR = Re::either(Re::single('-', '+'), Re::nothing());
static auto const RE_AT_KEYWORD = Re::chain('@'_re, RE_IDENTIFIER);

// non printable token https://www.w3.org/TR/css-syntax-3/#non-printable-code-point
static auto const RE_URL = Re::chain(
    Re::zeroOrOne(RE_WHITESPACE_TOKEN),
    Re::zeroOrMore(
        Re::either(
            RE_ESCAPE,
            Re::negate(
                Re::single('"', '\'', '(', ')', '\\', 0x007F, 0x000B) |
                RE_WHITESPACE | Re::range(0x0000, 0x0008) |
                Re::range(0x000E, 0x001F)
            )
        )
    ),
    Re::zeroOrOne(RE_WHITESPACE_TOKEN),
    RE_PARENTHESIS_CLOSE
);

static auto const RE_HASH = Re::chain(
    Re::single('#'),
    Re::oneOrMore(
        Re::either(
            RE_ESCAPE,
            Re::either(
                Re::alnum(),
                Re::single('-', '_'),
                Re::negate(RE_ASCII)
            )
        )
    )
);

// https://www.w3.org/TR/css-syntax-3/#consume-number
static auto const RE_NUMBER = Re::chain(
    RE_OPERATOR,
    Re::either(
        Re::chain(
            RE_DIGIT, Re::zeroOrOne(Re::chain(Re::single('.'), RE_DIGIT))
        ),
        Re::chain(Re::single('.'), RE_DIGIT)
    ),
    Re::zeroOrOne(
        Re::chain(
            Re::either(
                Re::single('e'),
                Re::single('E')
            ),
            RE_OPERATOR, RE_DIGIT
        )
    )
);

// string token description
static auto const RE_STRING =
    Re::either(
        Re::chain(
            '"'_re,
            Re::zeroOrMore(
                Re::either(
                    RE_ESCAPE,
                    Re::negate(
                        '\\'_re | RE_NEWLINE | '\"'_re
                    ),
                    '\\'_re & RE_NEWLINE
                )
            ),
            '"'_re
        ),
        Re::chain(
            '\''_re,
            Re::zeroOrMore(
                Re::either(
                    RE_ESCAPE,
                    Re::negate(
                        '\\'_re | RE_NEWLINE | '\''_re
                    ),
                    '\\'_re & RE_NEWLINE
                )
            ),
            '\''_re
        )
    );

export struct Lexer {
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

    Token _nextIdent(Io::SScan& s) const {
        if (not s.skip('('))
            return {Token::IDENT, s.end()};

        if (eqCi(s.end(), "url("s)) {
            if (s.ahead(Re::zeroOrMore(RE_WHITESPACE) & RE_QUOTES)) {
                return {Token::FUNCTION, s.end()};
            }
            s.skip(RE_URL);
            return {Token::URL, s.end()};
        }

        return {Token::FUNCTION, s.end()};
    }

    Token _next(Io::SScan& s) const {
        s.begin();
        if (s.ended()) {
            return {Token::END_OF_FILE, s.end()};
        } else if (s.skip(RE_WHITESPACE_TOKEN)) {
            return {Token::WHITESPACE, s.end()};
        } else if (s.skip(RE_BRACKET_OPEN)) {
            return {Token::LEFT_CURLY_BRACKET, s.end()};
        } else if (s.skip(RE_BRACKET_CLOSE)) {
            return {Token::RIGHT_CURLY_BRACKET, s.end()};
        } else if (s.skip(RE_SQUARE_BRACKET_OPEN)) {
            return {Token::LEFT_SQUARE_BRACKET, s.end()};
        } else if (s.skip(RE_SQUARE_BRACKET_CLOSE)) {
            return {Token::RIGHT_SQUARE_BRACKET, s.end()};
        } else if (s.skip(RE_PARENTHESIS_OPEN)) {
            return {Token::LEFT_PARENTHESIS, s.end()};
        } else if (s.skip(RE_PARENTHESIS_CLOSE)) {
            return {Token::RIGHT_PARENTHESIS, s.end()};
        } else if (s.skip(RE_SEMICOLON)) {
            return {Token::SEMICOLON, s.end()};
        } else if (s.skip(RE_COLON)) {
            return {Token::COLON, s.end()};
        } else if (s.skip(RE_COMMA)) {
            return {Token::COMMA, s.end()};
        } else if (s.skip(RE_HASH)) {
            return {Token::HASH, s.end()};
        } else if (s.skip("<!--")) {
            return {Token::CDO, s.end()};
        } else if (s.skip("-->")) {
            return {Token::CDC, s.end()};
        } else if (s.skip("/*")) {
            // https://www.w3.org/TR/css-syntax-3/#consume-comment
            s.skip(Re::untilAndConsume(Re::word("*/")));
            return {Token::COMMENT, s.end()};
        } else if (s.skip(RE_NUMBER)) {
            // https://www.w3.org/TR/css-syntax-3/#consume-numeric-token
            if (s.skip(RE_IDENTIFIER)) {
                return {Token::DIMENSION, s.end()};
            } else if (s.skip(Re::single('%'))) {
                return {Token::PERCENTAGE, s.end()};
            } else {
                return {Token::NUMBER, s.end()};
            }
        } else if (s.skip(RE_IDENTIFIER)) {
            return _nextIdent(s);
        } else if (s.skip(RE_AT_KEYWORD)) {
            return {Token::AT_KEYWORD, s.end()};
        } else if (s.skip(RE_STRING)) {
            // https://www.w3.org/TR/css-syntax-3/#consume-string-token
            return {Token::STRING, s.end()};
        } else if (s.skip(RE_DELIM)) {
            return {Token::DELIM, s.end()};
        } else {
            logWarn("unrecognized token: {}", s.end());
            s.next();
            return {Token::OTHER, s.end()};
        }
    }

    Token next() {
        auto res = _curr;
        _curr = _next(_scan);
        return res;
    }

    bool ended() const {
        return _scan.ended();
    }
};

// https://www.w3.org/TR/css-syntax-3/#consume-declaration
export void eatWhitespace(Lexer& lex) {
    while (lex.peek() == Token::WHITESPACE and not lex.ended())
        lex.next();
}

} // namespace Vaev::Css
