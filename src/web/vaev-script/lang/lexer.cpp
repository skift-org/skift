export module Vaev.Script:lang.lexer;

import Karm.Core;

using namespace Karm;

namespace Vaev::Script {

// 12.6 MARK: Tokens -----------------------------------------------------------
// https://tc39.es/ecma262/#sec-tokens

#define FOREACH_CATEGORY(CATEGORY) \
    CATEGORY(INVALID)              \
    CATEGORY(OPERATOR)             \
    CATEGORY(KEYWORD)              \
    CATEGORY(CONTROL_KEYWORD)      \
    CATEGORY(NUMBER)               \
    CATEGORY(IDENTIFIER)           \
    CATEGORY(STRING)               \
    CATEGORY(PUNCTUATION)

#define FOREACH_TOKEN(TOKEN)                        \
    TOKEN(INVALID, INVALID)                         \
    TOKEN(AMPERSAND, OPERATOR)                      \
    TOKEN(AMPERSAND_EQUALS, OPERATOR)               \
    TOKEN(ARROW, OPERATOR)                          \
    TOKEN(ASTERISK, OPERATOR)                       \
    TOKEN(ASTERISK_EQUALS, OPERATOR)                \
    TOKEN(ASYNC, KEYWORD)                           \
    TOKEN(AWAIT, KEYWORD)                           \
    TOKEN(BIG_INT_LITERAL, NUMBER)                  \
    TOKEN(BOOL_LITERAL, KEYWORD)                    \
    TOKEN(BRACKET_CLOSE, PUNCTUATION)               \
    TOKEN(BRACKET_OPEN, PUNCTUATION)                \
    TOKEN(BREAK, KEYWORD)                           \
    TOKEN(CARET, OPERATOR)                          \
    TOKEN(CARET_EQUALS, OPERATOR)                   \
    TOKEN(CASE, CONTROL_KEYWORD)                    \
    TOKEN(CATCH, CONTROL_KEYWORD)                   \
    TOKEN(CLASS, KEYWORD)                           \
    TOKEN(COLON, PUNCTUATION)                       \
    TOKEN(COMMA, PUNCTUATION)                       \
    TOKEN(CONST, KEYWORD)                           \
    TOKEN(CONTINUE, CONTROL_KEYWORD)                \
    TOKEN(CURLY_CLOSE, PUNCTUATION)                 \
    TOKEN(CURLY_OPEN, PUNCTUATION)                  \
    TOKEN(DEBUGGER, KEYWORD)                        \
    TOKEN(DEFAULT, CONTROL_KEYWORD)                 \
    TOKEN(DELETE, KEYWORD)                          \
    TOKEN(DO, CONTROL_KEYWORD)                      \
    TOKEN(DOUBLE_AMPERSAND, OPERATOR)               \
    TOKEN(DOUBLE_AMPERSAND_EQUALS, OPERATOR)        \
    TOKEN(DOUBLE_ASTERISK, OPERATOR)                \
    TOKEN(DOUBLE_ASTERISK_EQUALS, OPERATOR)         \
    TOKEN(DOUBLE_PIPE, OPERATOR)                    \
    TOKEN(DOUBLE_PIPE_EQUALS, OPERATOR)             \
    TOKEN(DOUBLE_QUESTION_MARK, OPERATOR)           \
    TOKEN(DOUBLE_QUESTION_MARK_EQUALS, OPERATOR)    \
    TOKEN(ELSE, CONTROL_KEYWORD)                    \
    TOKEN(ENUM, KEYWORD)                            \
    TOKEN(END_OF_FILE, INVALID)                     \
    TOKEN(EQUALS, OPERATOR)                         \
    TOKEN(EQUALS_EQUALS, OPERATOR)                  \
    TOKEN(EQUALS_EQUALS_EQUALS, OPERATOR)           \
    TOKEN(ESCAPED_KEYWORD, IDENTIFIER)              \
    TOKEN(EXCLAMATION_MARK, OPERATOR)               \
    TOKEN(EXCLAMATION_MARK_EQUALS, OPERATOR)        \
    TOKEN(EXCLAMATION_MARK_EQUALS_EQUALS, OPERATOR) \
    TOKEN(EXPORT, KEYWORD)                          \
    TOKEN(EXTENDS, KEYWORD)                         \
    TOKEN(FINALLY, CONTROL_KEYWORD)                 \
    TOKEN(FOR, CONTROL_KEYWORD)                     \
    TOKEN(FUNCTION, KEYWORD)                        \
    TOKEN(GREATER_THAN, OPERATOR)                   \
    TOKEN(GREATER_THAN_EQUALS, OPERATOR)            \
    TOKEN(IDENTIFIER, IDENTIFIER)                   \
    TOKEN(IF, CONTROL_KEYWORD)                      \
    TOKEN(IMPLEMENTS, KEYWORD)                      \
    TOKEN(IMPORT, KEYWORD)                          \
    TOKEN(IN, KEYWORD)                              \
    TOKEN(INSTANCEOF, KEYWORD)                      \
    TOKEN(INTERFACE, KEYWORD)                       \
    TOKEN(LESS_THAN, OPERATOR)                      \
    TOKEN(LESS_THAN_EQUALS, OPERATOR)               \
    TOKEN(LET, KEYWORD)                             \
    TOKEN(MINUS, OPERATOR)                          \
    TOKEN(MINUS_EQUALS, OPERATOR)                   \
    TOKEN(MINUS_MINUS, OPERATOR)                    \
    TOKEN(NEW, KEYWORD)                             \
    TOKEN(NULL_LITERAL, KEYWORD)                    \
    TOKEN(NUMERIC_LITERAL, NUMBER)                  \
    TOKEN(PACKAGE, KEYWORD)                         \
    TOKEN(PAREN_CLOSE, PUNCTUATION)                 \
    TOKEN(PAREN_OPEN, PUNCTUATION)                  \
    TOKEN(PERCENT, OPERATOR)                        \
    TOKEN(PERCENT_EQUALS, OPERATOR)                 \
    TOKEN(PERIOD, OPERATOR)                         \
    TOKEN(PIPE, OPERATOR)                           \
    TOKEN(PIPE_EQUALS, OPERATOR)                    \
    TOKEN(PLUS, OPERATOR)                           \
    TOKEN(PLUS_EQUALS, OPERATOR)                    \
    TOKEN(PLUS_PLUS, OPERATOR)                      \
    TOKEN(PRIVATE, KEYWORD)                         \
    TOKEN(PRIVATE_IDENTIFIER, IDENTIFIER)           \
    TOKEN(PROTECTED, KEYWORD)                       \
    TOKEN(PUBLIC, KEYWORD)                          \
    TOKEN(QUESTION_MARK, OPERATOR)                  \
    TOKEN(QUESTION_MARK_PERIOD, OPERATOR)           \
    TOKEN(REGEX_FLAGS, STRING)                      \
    TOKEN(REGEX_LITERAL, STRING)                    \
    TOKEN(RETURN, CONTROL_KEYWORD)                  \
    TOKEN(SEMICOLON, PUNCTUATION)                   \
    TOKEN(SHIFT_LEFT, OPERATOR)                     \
    TOKEN(SHIFT_LEFT_EQUALS, OPERATOR)              \
    TOKEN(SHIFT_RIGHT, OPERATOR)                    \
    TOKEN(SHIFT_RIGHT_EQUALS, OPERATOR)             \
    TOKEN(SLASH, OPERATOR)                          \
    TOKEN(SLASH_EQUALS, OPERATOR)                   \
    TOKEN(STATIC, KEYWORD)                          \
    TOKEN(STRING_LITERAL, STRING)                   \
    TOKEN(SUPER, KEYWORD)                           \
    TOKEN(SWITCH, CONTROL_KEYWORD)                  \
    TOKEN(TEMPLATE_LITERAL_END, STRING)             \
    TOKEN(TEMPLATE_LITERAL_EXPR_END, PUNCTUATION)   \
    TOKEN(TEMPLATE_LITERAL_EXPR_START, PUNCTUATION) \
    TOKEN(TEMPLATE_LITERAL_START, STRING)           \
    TOKEN(TEMPLATE_LITERAL_STRING, STRING)          \
    TOKEN(THIS, KEYWORD)                            \
    TOKEN(THROW, CONTROL_KEYWORD)                   \
    TOKEN(TILDE, OPERATOR)                          \
    TOKEN(TRIPLE_DOT, OPERATOR)                     \
    TOKEN(TRY, CONTROL_KEYWORD)                     \
    TOKEN(TYPEOF, KEYWORD)                          \
    TOKEN(UNSIGNED_SHIFT_RIGHT, OPERATOR)           \
    TOKEN(UNSIGNED_SHIFT_RIGHT_EQUALS, OPERATOR)    \
    TOKEN(UNTERMINATED_REGEX_LITERAL, STRING)       \
    TOKEN(UNTERMINATED_STRING_LITERAL, STRING)      \
    TOKEN(UNTERMINATED_TEMPLATE_LITERAL, STRING)    \
    TOKEN(VAR, KEYWORD)                             \
    TOKEN(VOID, KEYWORD)                            \
    TOKEN(WHILE, CONTROL_KEYWORD)                   \
    TOKEN(WITH, CONTROL_KEYWORD)                    \
    TOKEN(YIELD, CONTROL_KEYWORD)

struct Token {
    enum struct Category {
#define ITER(NAME) NAME,
        FOREACH_CATEGORY(ITER)
#undef ITER
            _LEN,
    };

    enum struct Type {
#define ITER(NAME, ...) NAME,
        FOREACH_TOKEN(ITER)
#undef ITER
            _LEN,
    };

    using enum Type;

    Type type = INVALID;
    Str data = "";

    Token() = default;

    Token(Type type, Str data = "")
        : type(type), data(data) {}

    Category category() const {
        switch (type) {
#define ITER(NAME, CAT) \
    case NAME:          \
        return Category::CAT;
            FOREACH_TOKEN(ITER)
#undef ITER
        default:
            panic("invalid token type");
        }
    }

    void repr(Io::Emit& e) const {
        e(
            "({}::{} {#})",
            category(),
            type,
            data
        );
    }
};

// MARK: Lexer -----------------------------------------------------------------

using Str2Token = Pair<Str, Token::Type>;

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

struct Lexer {
    Io::SScan& _scan;

    Lexer(Io::SScan& scan)
        : _scan(scan) {
    }

    Token _next(Io::SScan& s) const {
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

    Token next() {
        return _next(_scan);
    }

    Token peek() const {
        auto scan = _scan;
        return _next(scan);
    }

    bool ended() const {
        return _scan.ended();
    }
};

} // namespace Vaev::Script
