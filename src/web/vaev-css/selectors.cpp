#include "selectors.h"

namespace Vaev::Css {

// enum order is the operator priority (the lesser the most important)
enum struct OpCode {
    OR,         // ,
    DESCENDANT, // ' '
    CHILD,      // >
    ADJACENT,   // +
    SUBSEQUENT, // ~
    NOT,        // :not()
    WHERE,      // :where()
    AND,        // a.b
    COLUMN,     // ||
    NOP
};

Style::Selector parseSelectorElement(Slice<Sst> prefix, usize &i) {

    switch (prefix[i].token.type) {
    case Token::WHITESPACE:
        if (i + 1 >= prefix.len()) {
            // logError("ERROR : unterminated selector");
            return Style::EmptySelector{};
        }
        i++;
        return parseSelectorElement(prefix, i);
    case Token::HASH:
        return Style::IdSelector{next(prefix[i].token.data, 1)};
    case Token::IDENT:
        return Style::TypeSelector{TagName::make(prefix[i].token.data, Vaev::HTML)};
    case Token::DELIM:
        if (prefix[i].token.data == ".") {
            if (i + 1 >= prefix.len()) {
                logError("ERROR : unterminated selector");
                return Style::EmptySelector{};
            }
            i++;
            return Style::ClassSelector{prefix[i].token.data};
        } else if (prefix[i].token.data == "*") {
            return Style::UniversalSelector{};
        }
    case Token::COLON:
        if (i + 1 >= prefix.len()) {
            logError("ERROR : unterminated selector");
            return Style::EmptySelector{};
        }
        i++;
        if (prefix[i].token.type == Token::COLON) {
            if (i + 1 >= prefix.len()) {
                logError("ERROR : unterminated selector");
                return Style::EmptySelector{};
            }
            i++;
        }
        return Style::Pseudo{Style::Pseudo::make(prefix[i].token.data)};
    default:
        return Style::ClassSelector{prefix[i].token.data};
    }
}

OpCode sstNode2OpCode(Slice<Sst> content, usize &i) {
    switch (content[i].token.type) {
    case Token::COMMA:
        if (i + 1 >= content.len()) {
            logError("ERROR : unterminated selector");
            return OpCode::NOP;
        }
        i++;
        return OpCode::OR;
    case Token::WHITESPACE:

        // a white space could be an operator or be ignored if followed by another op
        if (i + 1 >= content.len()) {
            return OpCode::NOP;
        }

        if (content[i + 1] == Token::IDENT || content[i + 1] == Token::HASH || content[i + 1].token.data == "." || content[i + 1].token.data == "*") {
            i++;
            return OpCode::DESCENDANT;
        } else {
            i++;
            auto op = sstNode2OpCode(content, i);
            if (i >= content.len() - 1) {
                return OpCode::NOP;
            }
            if (content[i + 1].token.type == Token::WHITESPACE) {
                i++;
            }
            return op;
        }
    case Token::DELIM:
        if (i + 1 >= content.len()) {
            return OpCode::NOP;
        }
        if (content[i].token.data == ">") {
            i++;
            return OpCode::CHILD;
        } else if (content[i].token.data == "~") {
            i++;
            return OpCode::SUBSEQUENT;
        } else if (content[i].token.data == "+") {
            i++;
            return OpCode::ADJACENT;
        } else if (content[i].token.data == "." || content[i].token.data == "*") {
            return OpCode::AND;
        } else {
            return OpCode::NOP;
        }
    case Token::COLON:
    default:
        return OpCode::AND;
    }
}

Style::Selector parseInfixExpr(Style::Selector lhs, Slice<Sst> content, usize &i, OpCode opCode = OpCode::NOP);

Style::Selector parseNfixExpr(Style::Selector lhs, OpCode op, Slice<Sst> content, usize &i) {
    Vec<Style::Selector> selectors = {lhs, parseSelectorElement(content, i)};

    while (true) {

        if (i + 1 >= content.len()) {
            break;
        }
        usize rollBack = i;
        i++;

        OpCode nextOpCode = sstNode2OpCode(content, i);
        if (nextOpCode == OpCode::NOP) {
            break;
        } else if (nextOpCode == op) {
            // adding the selector to the nfix
            selectors.pushBack(parseSelectorElement(content, i));
        } else if (nextOpCode == OpCode::COLUMN || nextOpCode == OpCode::OR || nextOpCode == OpCode::AND) {
            // parse new nfix

            if (nextOpCode < op) {
                i = rollBack;
                break;
            }

            last(selectors) = parseNfixExpr(selectors[selectors.len() - 1], nextOpCode, content, i);
        } else {
            // parse new infix
            if (nextOpCode < op) {
                i = rollBack;
                break;
            }

            selectors.pushBack(parseInfixExpr(parseSelectorElement(content, i), content, i, nextOpCode));
        }
    }

    switch (op) {
    case OpCode::AND:
        return Style::Selector::and_(selectors);
    case OpCode::OR:
        return Style::Selector::or_(selectors);
    default:
        return Style::Selector::and_(selectors);
    }
}

Style::Selector parseInfixExpr(Style::Selector lhs, Slice<Sst> content, usize &i, OpCode opCode) {
    if (opCode == OpCode::NOP) {
        opCode = sstNode2OpCode(content, i);
    }

    switch (opCode) {
    case OpCode::NOP:
        return lhs;
    case OpCode::DESCENDANT:
        return Style::Selector::descendant(lhs, parseSelectorElement(content, i));
    case OpCode::CHILD:
        return Style::Selector::child(lhs, parseSelectorElement(content, i));
    case OpCode::ADJACENT:
        return Style::Selector::adjacent(lhs, parseSelectorElement(content, i));
    case OpCode::SUBSEQUENT:
        return Style::Selector::subsequent(lhs, parseSelectorElement(content, i));
    case OpCode::NOT:
        return Style::Selector::not_(parseSelectorElement(content, i));
    case OpCode::WHERE:
        return Style::Selector::where(parseSelectorElement(content, i));
    case OpCode::COLUMN:
    case OpCode::OR:
    case OpCode::AND:
        return parseNfixExpr(lhs, opCode, content, i);
    }
}

Style::Selector parseSelector(Slice<Sst> content) {
    if (!content) {
        logWarn("empty selector");
        return Style::EmptySelector{};
    }

    usize i = 0;
    Style::Selector currentSelector = parseSelectorElement(content, i);
    while (i + 1 < content.len()) {
        i++;
        currentSelector = parseInfixExpr(currentSelector, content, i);
    }
    return currentSelector;
}
} // namespace Vaev::Css
