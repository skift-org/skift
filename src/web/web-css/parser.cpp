#include "parser.h"

namespace Web::Css {

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-rules
Content consumeRuleList(Lexer &lex, bool topLevel) {
    Content list{};

    while (true) {
        switch (lex.peek().type) {
        case Token::COMMENT:
        case Token::WHITESPACE:
            lex.next();
            break;

        case Token::END_OF_FILE:
            lex.next();
            return list;

        case Token::CDC:
        case Token::CDO: {
            if (not topLevel) {
                auto rule = consumeQualifiedRule(lex);
                if (rule)
                    list.pushBack(*rule);
            }
            break;
        }

        case Token::AT_KEYWORD: {
            list.pushBack(consumeAtRule(lex));
            break;
        }

        default: {
            auto rule = consumeQualifiedRule(lex);
            if (rule)
                list.pushBack(*rule);
            break;
        }
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-at-rule
Sst consumeAtRule(Lexer &lex) {
    Sst atRule{Sst::RULE};
    atRule.token = lex.peek();
    Content prefix;

    while (true) {
        switch (lex.peek().type) {
        case Token::SEMICOLON:
            lex.next();
            atRule.prefix = prefix;
            return atRule;

        case Token::END_OF_FILE:
            logError("unexpected end of file");

            lex.next();
            atRule.prefix = prefix;
            return atRule;

        case Token::LEFT_CURLY_BRACKET:
            atRule.content = consumeStyleBlock(lex);
            atRule.prefix = prefix;
            return atRule;

        default:
            prefix.pushBack(consumeComponentValue(lex));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Opt<Sst> consumeQualifiedRule(Lexer &lex) {
    Sst rule{Sst::RULE};
    Content prefix;

    while (true) {
        switch (lex.peek().type) {
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            lex.next();
            return rule;

        case Token::LEFT_CURLY_BRACKET: {
            rule.prefix = std::move(prefix);
            rule.content.emplaceBack(consumeBlock(lex, Token::RIGHT_CURLY_BRACKET));
            return rule;
        }

        default:
            prefix.pushBack(consumeComponentValue(lex));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-style-block
Content consumeStyleBlock(Lexer &lex) {
    Content block;
    lex.next();

    while (true) {
        switch (lex.peek().type) {
        case Token::WHITESPACE:
        case Token::SEMICOLON:
            lex.next();
            break;

        case Token::END_OF_FILE:
            logError("unexpected end of file");
            lex.next();
            return block;

        case Token::AT_KEYWORD:
            block.pushBack(consumeAtRule(lex));
            break;

        case Token::IDENT:
            if (lex.peek().data == "&") {
                auto rule = consumeQualifiedRule(lex);
                if (rule)
                    block.pushBack(*rule);
            } else {
                auto decl = consumeDeclaration(lex);
                if (decl)
                    block.pushBack(*decl);
            }
            break;

        default:
            logError("unexpected token");
            if (not(lex.peek() == Token::SEMICOLON or lex.peek() == Token::END_OF_FILE))
                consumeComponentValue(lex);
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-declarations
Sst consumeDeclarationList(Lexer &lex) {
    Content list;
    lex.next();

    while (true) {
        switch (lex.peek().type) {
        case Token::WHITESPACE:
        case Token::SEMICOLON:
            lex.next();
            break;

        case Token::END_OF_FILE:
            logError("unexpected end of file");
            lex.next();
            return list;

        case Token::AT_KEYWORD:
            list.pushBack(consumeAtRule(lex));
            break;

        case Token::IDENT: {
            auto decl = consumeDeclaration(lex);
            if (decl)
                list.pushBack(*decl);
            break;
        }

        default:
            logError("unexpected token");
            if (not(lex.peek() == Token::SEMICOLON or lex.peek() == Token::END_OF_FILE))
                consumeComponentValue(lex);
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-declaration
Opt<Sst> consumeDeclaration(Lexer &lex) {
    Sst decl{Sst::DECL};
    decl.token = lex.next();

    // 1. While the next input token is a <whitespace-token>, consume the next input token.
    while (lex.peek() == Token::WHITESPACE)
        lex.next();

    // 2. If the next input token is anything other than a <colon-token>, this is a parse error. Return nothing.
    if (lex.peek() != Token::COLON) {
        logError("expected colon");
        return NONE;
    }

    // Otherwise, consume the next input token.
    lex.next();

    // 3. While the next input token is a <whitespace-token>, consume the next input token.
    while (lex.peek() == Token::WHITESPACE)
        lex.next();

    // 4. As long as the next input token is anything other than an <EOF-token>,
    //    consume a component value and append it to the declaration’s value.
    while ((lex.peek() != Token::END_OF_FILE and lex.peek() != Token::SEMICOLON)) {
        decl.content.pushBack(consumeComponentValue(lex));
        while (lex.peek() == Token::WHITESPACE)
            lex.next();
    }

    return decl;
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
Sst consumeComponentValue(Lexer &lex) {
    switch (lex.peek().type) {
    case Token::LEFT_SQUARE_BRACKET:
        return consumeBlock(lex, Token::RIGHT_SQUARE_BRACKET);

    case Token::LEFT_CURLY_BRACKET:
        return consumeBlock(lex, Token::RIGHT_CURLY_BRACKET);

    case Token::LEFT_PARENTHESIS:
        return consumeBlock(lex, Token::RIGHT_PARENTHESIS);

    case Token::FUNCTION:
        return consumeFunc(lex);

    default:
        return lex.next();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Sst consumeBlock(Lexer &lex, Token::Type term) {
    Sst block = Sst::BLOCK;
    lex.next();

    while (true) {
        auto token = lex.peek();

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
    fn.prefix = lex.next();

    while (true) {
        switch (lex.peek().type) {
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            return fn;

        case Token::RIGHT_PARENTHESIS:
            lex.next();
            return fn;

        default:
            fn.content.pushBack(consumeComponentValue(lex));
            break;
        }
    }
}

} // namespace Web::Css
