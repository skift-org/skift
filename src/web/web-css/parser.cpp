#include "parser.h"

namespace Web::Css {

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Sst consumeBlock(Lexer &lex, Token::Type term) {
    Sst block = Sst::BLOCK;
    lex.next();

    while (true) {
        auto token = lex.curr();

        switch (token.type) {
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            return block;

        default:
            if (token.type == term)
                return block;

            block.content.emplaceBack(consumeComponentValue(lex));
            break;
        }

        lex.next();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-function
Sst consumeFunc(Lexer &lex) {
    Sst fn = Sst::FUNC;
    fn.prefix = Box<Sst>{lex.curr()};
    lex.next();

    while (true) {
        switch (lex.curr().type) {
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            return fn;

        case Token::RIGHT_PARENTHESIS:
            return fn;

        default:
            fn.content.pushBack(consumeComponentValue(lex));
            break;
        }

        lex.next();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
Sst consumeComponentValue(Lexer &lex) {
    switch (lex.curr().type) {
    case Token::LEFT_SQUARE_BRACKET:
        return consumeBlock(lex, Token::RIGHT_SQUARE_BRACKET);

    case Token::LEFT_CURLY_BRACKET:
        return consumeBlock(lex, Token::RIGHT_CURLY_BRACKET);

    case Token::LEFT_PARENTHESIS:
        return consumeBlock(lex, Token::RIGHT_PARENTHESIS);

    case Token::FUNCTION:
        return consumeFunc(lex);

    default:
        return lex.curr();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Sst consumeQualifiedRule(Lexer &lex) {
    Sst pre{Sst::LIST};
    while (true) {
        switch (lex.curr().type) {
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            return pre;

        case Token::LEFT_CURLY_BRACKET: {
            Sst ast{Sst::QUALIFIED_RULE};
            ast.prefix = Box{pre};
            ast.content.emplaceBack(consumeBlock(lex, Token::RIGHT_CURLY_BRACKET));
            return ast;
        }

        default:
            pre.content.pushBack(consumeComponentValue(lex));
            break;
        }

        lex.next();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-rules
Sst consumeRuleList(Lexer &lex) {
    Sst ast = Sst::LIST;

    while (true) {
        switch (lex.curr().type) {
        case Token::END_OF_FILE:
            return ast;

        case Token::COMMENT:
        case Token::WHITESPACE:
            break;

        default:
            ast.content.pushBack(consumeQualifiedRule(lex));
            break;
        }

        lex.next();
    }
}

} // namespace Web::Css
