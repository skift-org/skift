#include <karm-base/array.h>

#include "lexer.h"

namespace Vaev::Js {

// MARK: Token -----------------------------------------------------------------

void Token::repr(Io::Emit &e) const {
    e(
        "({}::{} {#})",
        category(),
        type,
        data
    );
}

// MARK: Lexer -----------------------------------------------------------------

using Str2Token = Cons<Str, Token::Type>;

static constexpr auto _singleCharTokens = [] {
    Array<Token::Type, 256> res{};
    res['&'] = Token::AMPERSAND;
    res['*'] = Token::ASTERISK;
    res['['] = Token::BRACKET_OPEN;
    res[']'] = Token::BRACKET_CLOSE;
    res['^'] = Token::CARET;
    res[':'] = Token::COLON;
    res[','] = Token::COMMA;
    res['{'] = Token::CURLY_OPEN;
    res['}'] = Token::CURLY_CLOSE;
    res['='] = Token::EQUALS;
    res['!'] = Token::EXCLAMATION_MARK;
    res['-'] = Token::MINUS;
    res['('] = Token::PAREN_OPEN;
    res[')'] = Token::PAREN_CLOSE;
    res['%'] = Token::PERCENT;
    res['.'] = Token::PERIOD;
    res['|'] = Token::PIPE;
    res['+'] = Token::PLUS;
    res['?'] = Token::QUESTION_MARK;
    res[';'] = Token::SEMICOLON;
    res['/'] = Token::SLASH;
    res['~'] = Token::TILDE;
    res['<'] = Token::LESS_THAN;
    res['>'] = Token::GREATER_THAN;
    return res;
}();

static constexpr Array _twoCharTokens = {
    Str2Token{"=>", Token::ARROW},
    Str2Token{"+=", Token::PLUS_EQUALS},
    Str2Token{"-=", Token::MINUS_EQUALS},
    Str2Token{"*=", Token::ASTERISK_EQUALS},
    Str2Token{"/=", Token::SLASH_EQUALS},
    Str2Token{"%=", Token::PERCENT_EQUALS},
    Str2Token{"&=", Token::AMPERSAND_EQUALS},
    Str2Token{"|=", Token::PIPE_EQUALS},
    Str2Token{"^=", Token::CARET_EQUALS},
    Str2Token{"&&", Token::DOUBLE_AMPERSAND},
    Str2Token{"||", Token::DOUBLE_PIPE},
    Str2Token{"??", Token::DOUBLE_QUESTION_MARK},
    Str2Token{"**", Token::DOUBLE_ASTERISK},
    Str2Token{"==", Token::EQUALS_EQUALS},
    Str2Token{"<=", Token::LESS_THAN_EQUALS},
    Str2Token{">=", Token::GREATER_THAN_EQUALS},
    Str2Token{"!=", Token::EXCLAMATION_MARK_EQUALS},
    Str2Token{"--", Token::MINUS_MINUS},
    Str2Token{"++", Token::PLUS_PLUS},
    Str2Token{"<<", Token::SHIFT_LEFT},
    Str2Token{">>", Token::SHIFT_RIGHT},
    Str2Token{"?.", Token::QUESTION_MARK_PERIOD},
};

static constexpr Array _threeCharTokens = {
    Str2Token{"===", Token::EQUALS_EQUALS_EQUALS},
    Str2Token{"!==", Token::EXCLAMATION_MARK_EQUALS_EQUALS},
    Str2Token{"**=", Token::DOUBLE_ASTERISK_EQUALS},
    Str2Token{"<<=", Token::SHIFT_LEFT_EQUALS},
    Str2Token{">>=", Token::SHIFT_RIGHT_EQUALS},
    Str2Token{"&&=", Token::DOUBLE_AMPERSAND_EQUALS},
    Str2Token{"||=", Token::DOUBLE_PIPE_EQUALS},
    Str2Token{"?\?=", Token::DOUBLE_QUESTION_MARK_EQUALS},
    Str2Token{">>>", Token::UNSIGNED_SHIFT_RIGHT},
    Str2Token{"...", Token::TRIPLE_DOT},
};

static constexpr Array _fourCharTokens = {
    Str2Token{">>>=", Token::UNSIGNED_SHIFT_RIGHT_EQUALS},
};

static constexpr Array _keywordTokens = {
    Str2Token{"async", Token::ASYNC},
    Str2Token{"await", Token::AWAIT},
    Str2Token{"break", Token::BREAK},
    Str2Token{"case", Token::CASE},
    Str2Token{"catch", Token::CATCH},
    Str2Token{"class", Token::CLASS},
    Str2Token{"const", Token::CONST},
    Str2Token{"continue", Token::CONTINUE},
    Str2Token{"debugger", Token::DEBUGGER},
    Str2Token{"default", Token::DEFAULT},
    Str2Token{"delete", Token::DELETE},
    Str2Token{"do", Token::DO},
    Str2Token{"else", Token::ELSE},
    Str2Token{"enum", Token::ENUM},
    Str2Token{"export", Token::EXPORT},
    Str2Token{"extends", Token::EXTENDS},
    Str2Token{"false", Token::BOOL_LITERAL},
    Str2Token{"finally", Token::FINALLY},
    Str2Token{"for", Token::FOR},
    Str2Token{"function", Token::FUNCTION},
    Str2Token{"if", Token::IF},
    Str2Token{"import", Token::IMPORT},
    Str2Token{"in", Token::IN},
    Str2Token{"instanceof", Token::INSTANCEOF},
    Str2Token{"let", Token::LET},
    Str2Token{"new", Token::NEW},
    Str2Token{"null", Token::NULL_LITERAL},
    Str2Token{"return", Token::RETURN},
    Str2Token{"super", Token::SUPER},
    Str2Token{"switch", Token::SWITCH},
    Str2Token{"this", Token::THIS},
    Str2Token{"throw", Token::THROW},
    Str2Token{"true", Token::BOOL_LITERAL},
    Str2Token{"try", Token::TRY},
    Str2Token{"typeof", Token::TYPEOF},
    Str2Token{"var", Token::VAR},
    Str2Token{"void", Token::VOID},
    Str2Token{"while", Token::WHILE},
    Str2Token{"with", Token::WITH},
    Str2Token{"yield", Token::YIELD},
};

Token Lexer::_next(Io::SScan &s) const {
    if (s.ended())
        return Token::END_OF_FILE;

    s.begin();

    // NOSPEC: try keywords
    for (auto [str, token] : _keywordTokens) {
        if (s.ahead(str)) {
            s.skip(str);
            return {token, s.end()};
        }
    }

    // try 4-char tokens
    for (auto [str, token] : _fourCharTokens) {
        if (s.ahead(str)) {
            s.skip(str);
            return {token, s.end()};
        }
    }

    // try 3-char tokens
    for (auto [str, token] : _threeCharTokens) {
        if (s.ahead(str)) {
            s.skip(str);
            return {token, s.end()};
        }
    }

    // try 2-char tokens
    for (auto [str, token] : _twoCharTokens) {
        if (s.ahead(str)) {
            s.skip(str);
            return {token, s.end()};
        }
    }

    // try 1-char tokens
    if (auto c = s.peek(); c < _singleCharTokens.len()) {
        if (auto token = _singleCharTokens[c]; token != Token::INVALID) {
            s.next();
            return {token, s.end()};
        }
    }

    s.next();
    return {Token::INVALID, s.end()};
}

} // namespace Vaev::Js
