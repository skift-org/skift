#include "parser.h"

namespace Web::Css {

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Res<Sst> consumeBlock(Io::SScan &s, Token::Type term) {
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
                auto a = try$(consumeComponentValue(s, token));
                logDebug("adding {#} to the curent block", a);
                block.content.emplaceBack(a);
            }
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-function
Res<Sst> consumeFunc(Io::SScan &s, Token token) {
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
            fn.content.emplaceBack(try$(consumeComponentValue(s, token)));
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
Res<Sst> consumeComponentValue(Io::SScan &s, Token token) {
    switch (token.type) {
    case Token::LEFT_SQUARE_BRACKET:
        return consumeBlock(s, Token::RIGHT_SQUARE_BRACKET);

    case Token::LEFT_CURLY_BRACKET:
        return consumeBlock(s, Token::RIGHT_CURLY_BRACKET);

    case Token::LEFT_PARENTHESIS:
        return consumeBlock(s, Token::RIGHT_PARENTHESIS);

    case Token::FUNCTION:
        return consumeFunc(s, token);

    default:
        Sst tok = Sst::TOKEN;
        tok.token = token;
        return Ok(std::move(tok));
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Res<Sst> consumeQualifiedRule(Io::SScan &s, Token token) {
    Sst ast{Sst::QUALIFIED_RULE};
    Sst pre{Sst::LIST};

    while (true) {
        switch (token.type) {
        case Token::END_OF_FILE:
            return Error();
        case Token::LEFT_CURLY_BRACKET:
            logDebug("creating a block for curent rule");
            ast.prefix = Box{pre};
            ast.content.emplaceBack(try$(consumeBlock(s, Token::RIGHT_CURLY_BRACKET)));
            return Ok(ast);
        default:
            auto a = try$(consumeComponentValue(s, token));
            logDebug("adding a new component value to the prefix {#}", a);
            pre.content.pushBack(a);
            break;
        }
        token = nextToken(s).unwrap();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-rules
Res<Sst> consumeRuleList(Io::SScan &s) {
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
            ast.content.pushBack(try$(consumeQualifiedRule(s, token)));
            break;
        }
    }
}

} // namespace Web::Css
