#include <karm-base/array.h>
#include <karm-base/cons.h>
#include <karm-io/expr.h>
#include <karm-logger/logger.h>

#include "lexer.h"

namespace Web::Css {

static auto const RE_BRACKET_OPEN = Re::single('{');
static auto const RE_BRACKET_CLOSE = Re::single('}');
static auto const RE_PARENTHESIS_OPEN = Re::single('(');
static auto const RE_PARENTHESIS_CLOSE = Re::single(')');
static auto const RE_SQUARE_BRACKET_OPEN = Re::single('[');
[[maybe_unused]] static auto const RE_SQUARE_BRACKET_CLOSE = Re::single(']');
static auto const RE_SEMICOLON = Re::single(';');
static auto const RE_COLON = Re::single(':');
static auto const RE_COMMA = Re::single(',');

// NOSPEC: In the spec the fallback is delim
// since DELIM has a semantic meaning and could catch a really unexpected content
// we chose to use OTHER as a fallback and delim is a determined set of characters
static auto const RE_DELIM = '.'_re | '+'_re | '-'_re | '#'_re | '~'_re | '!'_re | '*'_re;

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

static auto const RE_FUNCTION = RE_IDENTIFIER & RE_PARENTHESIS_OPEN;

// non printable token https://www.w3.org/TR/css-syntax-3/#non-printable-code-point
static auto const RE_URL = Re::chain(
    Re::zeroOrOne(RE_WHITESPACE_TOKEN),
    Re::zeroOrMore(
        Re::either(RE_ESCAPE, Re::negate(Re::single('"', '\'', '(', ')', '\\', 0x007F, 0x000B) | RE_WHITESPACE | Re::range(0x0000, 0x0008) | Re::range(0x000E, 0x001F)))
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
    Re::zeroOrOne(Re::chain(
        Re::either(Re::single('e'), Re::single('E')),
        RE_OPERATOR,
        RE_DIGIT
    ))
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

Res<Token> nextToken(Io::SScan &s) {
    if (s.ended()) {
        // NO SPEC
        return Ok(Token{Token::END_OF_FILE, s.end()});
    }

    s.begin();

    if (s.skip(RE_WHITESPACE_TOKEN)) {
        return Ok(Token{Token::WHITESPACE, s.end()});
    }

    if (s.skip(RE_BRACKET_OPEN)) {
        return Ok(Token{Token::LEFT_CURLY_BRACKET, s.end()});
    }

    if (s.skip(RE_BRACKET_CLOSE)) {
        return Ok(Token{Token::RIGHT_CURLY_BRACKET, s.end()});
    }

    if (s.skip(RE_SQUARE_BRACKET_OPEN)) {
        return Ok(Token{Token::LEFT_SQUARE_BRACKET, s.end()});
    }

    if (s.skip(RE_SQUARE_BRACKET_CLOSE)) {
        return Ok(Token{Token::RIGHT_SQUARE_BRACKET, s.end()});
    }

    if (s.skip(RE_PARENTHESIS_OPEN)) {
        return Ok(Token{Token::LEFT_PARENTHESIS, s.end()});
    }

    if (s.skip(RE_PARENTHESIS_CLOSE)) {
        return Ok(Token{Token::RIGHT_PARENTHESIS, s.end()});
    }

    if (s.skip(RE_SEMICOLON)) {
        return Ok(Token{Token::SEMICOLON, s.end()});
    }

    if (s.skip(RE_COLON)) {
        return Ok(Token{Token::COLON, s.end()});
    }

    if (s.skip(RE_COMMA)) {
        return Ok(Token{Token::COMMA, s.end()});
    }

    if (s.skip(RE_HASH)) {
        return Ok(Token{Token::HASH, s.end()});
    }

    // https://www.w3.org/TR/css-syntax-3/#consume-comment
    if (s.skip("/*")) {
        s.skip(Re::untilAndConsume(Re::word("*/")));

        Str data = s.end();

        if (endWith(data, Str{"*/"}) == Match::NO) {
            return Error::invalidInput("unterminated comment");
        }

        return Ok(Token{Token::COMMENT, data});
    }

    // https://www.w3.org/TR/css-syntax-3/#consume-numeric-token
    if (s.skip(RE_NUMBER)) {
        if (s.skip(RE_IDENTIFIER)) {
            return Ok(Token{Token::DIMENSION, s.end()});
        }

        if (s.skip(Re::single('%'))) {
            return Ok(Token{Token::PERCENTAGE, s.end()});
        }

        return Ok(Token{Token::NUMBER, s.end()});
    }

    if (s.skip(RE_FUNCTION)) {
        if (s.end() == "url(" and s.skip(RE_URL)) {
            // unclear spec
            return Ok(Token{Token::URL, s.end()});
        }

        return Ok(Token{Token::FUNCTION, s.end()});
    }

    // https://www.w3.org/TR/css-syntax-3/#consume-name
    if (s.skip(RE_IDENTIFIER)) {
        return Ok(Token{Token::IDENT, s.end()});
    }

    if (s.skip(RE_AT_KEYWORD)) {
        return Ok(Token{Token::AT_KEYWORD, s.end()});
    }

    // https://www.w3.org/TR/css-syntax-3/#consume-string-token
    if (s.skip(RE_STRING)) {
        return Ok(Token{Token::STRING, s.end()});
    }

    if (s.skip(RE_DELIM)) {
        return Ok(Token{Token::DELIM, s.end()});
    }

    logDebug("error at {#}", s.curr());
    // NO SPEC BUT SHOULD BE UNREACHABLE
    s.next();
    return Ok(Token{Token::OTHER, s.end()});
}

} // namespace Web::Css
