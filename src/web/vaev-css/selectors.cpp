#include "selectors.h"

#include "vaev-css/values.h"

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

Style::Selector parseAttributeSelector(Slice<Sst> content) {
    auto caze = Style::AttributeSelector::INSENSITIVE;
    Str name = "";
    String value = ""s;
    auto match = Style::AttributeSelector::Match{Style::AttributeSelector::PRESENT};

    usize step = 0;
    Cursor<Sst> cur = Cursor<Sst>(content);
    while (not cur.ended() and cur->token.data != "]"s) {
        if (cur->token == Token::WHITESPACE) {
            cur.next();
            continue;
        }

        switch (step) {
        case 0:
            name = cur->token.data;
            step++;
            break;
        case 1:
            if (cur->token.data != "="s) {
                if (cur.ended() or cur.peek(1).token.data != "="s) {
                    break;
                }
                if (cur->token.data == "~") {
                    match = Style::AttributeSelector::Match{Style::AttributeSelector::CONTAINS};
                } else if (cur->token.data == "|") {
                    match = Style::AttributeSelector::Match{Style::AttributeSelector::HYPHENATED};
                } else if (cur->token.data == "^") {
                    match = Style::AttributeSelector::Match{Style::AttributeSelector::STR_START_WITH};
                } else if (cur->token.data == "$") {
                    match = Style::AttributeSelector::Match{Style::AttributeSelector::STR_END_WITH};
                } else if (cur->token.data == "*") {
                    match = Style::AttributeSelector::Match{Style::AttributeSelector::STR_CONTAIN};
                } else {
                    break;
                }
                cur.next();
            } else {
                match = Style::AttributeSelector::Match{Style::AttributeSelector::EXACT};
            }
            step++;
            break;
        case 2:
            value = parseValue<String>(cur).unwrap();
            step++;
            break;
        case 3:
            if (cur->token.data == "s") {
                caze = Style::AttributeSelector::SENSITIVE;
            }
            break;
        }

        if (cur.ended()) {
            break;
        }
        cur.next();
    }
    return Style::AttributeSelector{name, caze, match, value};
}

Style::Selector parseSelectorElement(Cursor<Sst> &cur) {
    Style::Selector val;

    if (*cur == Sst::TOKEN) {
        switch (cur->token.type) {
        case Token::WHITESPACE:
            if (cur.ended()) {
                // logError("ERROR : unterminated selector");
                return Style::EmptySelector{};
            }
            cur.next();
            return parseSelectorElement(cur);
        case Token::HASH:
            if (cur.ended()) {
                // logError("ERROR : unterminated selector");
                return Style::EmptySelector{};
            }
            val = Style::IdSelector{next(cur->token.data, 1)};
            break;
        case Token::IDENT:
            val = Style::TypeSelector{TagName::make(cur->token.data, Vaev::HTML)};
            break;
        case Token::DELIM:
            if (cur->token.data == ".") {
                cur.next();
                if (cur.ended()) {
                    logError("ERROR : unterminated selector");
                    return Style::EmptySelector{};
                }
                val = Style::ClassSelector{cur->token.data};
            } else if (cur->token.data == "*") {
                val = Style::UniversalSelector{};
            }
            break;
        case Token::COLON:
            cur.next();
            if (cur.ended()) {
                logError("ERROR : unterminated selector");
                return Style::EmptySelector{};
            }
            logDebug("First COLON eated {}", cur.peek());
            if (cur->token.type == Token::COLON) {
                cur.next();
                if (cur.ended()) {
                    logError("ERROR : unterminated selector");
                    return Style::EmptySelector{};
                }
            }
            val = Style::Pseudo{Style::Pseudo::make(cur->token.data)};
            break;
        default:
            val = Style::ClassSelector{cur->token.data};
            break;
        }
    } else if (cur->type == Sst::Type::BLOCK) {
        val = parseAttributeSelector(cur->content);
    } else {
        return Style::EmptySelector{};
    }

    cur.next();
    return val;
}

OpCode peekOpCode(Cursor<Sst> &cur) {
    if (cur->type == Sst::Type::TOKEN) {

        switch (cur->token.type) {
        case Token::COMMA:
            if (cur.ended()) {
                return OpCode::NOP;
            }
            cur.next();
            return OpCode::OR;
        case Token::WHITESPACE:
            cur.next();
            // a white space could be an operator or be ignored if followed by another op
            if (cur.ended()) {
                return OpCode::NOP;
            }

            if (
                cur.peek() == Token::IDENT or
                cur.peek() == Token::HASH or
                cur.peek().token.data == "." or
                cur.peek().token.data == "*"
            ) {
                return OpCode::DESCENDANT;
            } else {
                auto op = peekOpCode(cur);

                if (cur.peek(1).token.type == Token::WHITESPACE) {
                    if (cur.ended()) {
                        return OpCode::NOP;
                    }
                    cur.next();
                }
                return op;
            }
        case Token::DELIM:
            if (cur.rem() <= 1) {
                return OpCode::NOP;
            }
            if (cur->token.data == ">") {
                if (cur.ended()) {
                    return OpCode::NOP;
                }
                cur.next();
                return OpCode::CHILD;
            } else if (cur->token.data == "~") {
                if (cur.ended()) {
                    return OpCode::NOP;
                }
                cur.next();
                return OpCode::SUBSEQUENT;
            } else if (cur->token.data == "+") {
                if (cur.ended()) {
                    return OpCode::NOP;
                }
                cur.next();
                return OpCode::ADJACENT;
            } else if (cur->token.data == "." or cur->token.data == "*") {
                return OpCode::AND;
            } else {
                return OpCode::NOP;
            }
        case Token::COLON:
            if (cur.ended()) {
                return OpCode::NOP;
            }
        default:
            return OpCode::AND;
        }
    } else {
        return OpCode::AND;
    }
}

Style::Selector parseInfixExpr(Style::Selector lhs, Cursor<Sst> &cur, OpCode opCode = OpCode::NOP);

Style::Selector parseNfixExpr(Style::Selector lhs, OpCode op, Cursor<Sst> &cur) {
    Vec<Style::Selector> selectors = {lhs, parseSelectorElement(cur)};
    while (true) {
        if (cur.ended()) {
            break;
        }

        Cursor<Sst> rollBack = cur;

        OpCode nextOpCode = peekOpCode(cur);

        if (nextOpCode == OpCode::NOP) {
            break;
        } else if (nextOpCode == op) {
            // adding the selector to the nfix
            selectors.pushBack(parseSelectorElement(cur));
        } else if (nextOpCode == OpCode::COLUMN or nextOpCode == OpCode::OR or nextOpCode == OpCode::AND) {
            // parse new nfix

            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }

            last(selectors) = parseNfixExpr(last(selectors), nextOpCode, cur);
        } else {
            // parse new infix
            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }

            selectors.pushBack(parseInfixExpr(parseSelectorElement(cur), cur, nextOpCode));
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

Style::Selector parseInfixExpr(Style::Selector lhs, Cursor<Sst> &cur, OpCode opCode) {

    if (opCode == OpCode::NOP) {
        opCode = peekOpCode(cur);
    }

    switch (opCode) {
    case OpCode::NOP:
        return lhs;
    case OpCode::DESCENDANT:
        return Style::Selector::descendant(lhs, parseSelectorElement(cur));
    case OpCode::CHILD:
        return Style::Selector::child(lhs, parseSelectorElement(cur));
    case OpCode::ADJACENT:
        return Style::Selector::adjacent(lhs, parseSelectorElement(cur));
    case OpCode::SUBSEQUENT:
        return Style::Selector::subsequent(lhs, parseSelectorElement(cur));
    case OpCode::NOT:
        return Style::Selector::not_(parseSelectorElement(cur));
    case OpCode::WHERE:
        return Style::Selector::where(parseSelectorElement(cur));
    case OpCode::COLUMN:
    case OpCode::OR:
    case OpCode::AND:
        return parseNfixExpr(lhs, opCode, cur);
    }
}

Style::Selector parseSelector(Slice<Sst> content) {
    if (!content) {
        logWarn("empty selector");
        return Style::EmptySelector{};
    }

    auto cur = Cursor<Sst>{content};
    Style::Selector currentSelector = parseSelectorElement(cur);

    while (not cur.ended()) {
        currentSelector = parseInfixExpr(currentSelector, cur);
    }
    return currentSelector;
}

} // namespace Vaev::Css
