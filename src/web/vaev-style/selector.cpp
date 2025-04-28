#include "selector.h"

#include "values.h"

namespace Vaev::Style {
static constexpr bool DEBUG_SELECTORS = false;

// MARK: Selector Specificity ---------------------------------------------------

// https://www.w3.org/TR/selectors-3/#specificity
Spec spec(Selector const& s) {
    return s.visit(Visitor{[](Nfix const& n) {
                               // FIXME: missing other pseudo class selectors implemented as nfix
                               if (n.type == Nfix::WHERE)
                                   return Spec::ZERO;

                               Spec sum = Spec::ZERO;
                               for (auto& inner : n.inners)
                                   sum = sum + spec(inner);
                               return sum;
                           },
                           [](Infix const& i) {
                               return spec(*i.lhs) + spec(*i.rhs);
                           },
                           [](UniversalSelector const&) {
                               return Spec::ZERO;
                           },
                           [](EmptySelector const&) {
                               return Spec::ZERO;
                           },
                           [](IdSelector const&) {
                               return Spec::A;
                           },
                           [](TypeSelector const&) {
                               return Spec::C;
                           },
                           [](ClassSelector const&) {
                               return Spec::B;
                           },
                           [](Pseudo const&) {
                               return Spec::B;
                           },
                           [](AttributeSelector const&) {
                               return Spec::B;
                           },
                           [](auto const& s) {
                               logWarnIf(DEBUG_SELECTORS, "unimplemented selector: {}", s);
                               return Spec::ZERO;
                           }});
}

// MARK: Parser ----------------------------------------------------------------

// enum order is the operator priority (the lesser the most important)
enum struct OpCode {
    NOP,
    OR,         // ,
    DESCENDANT, // ' '
    CHILD,      // >
    ADJACENT,   // +
    SUBSEQUENT, // ~
    NOT,        // :not()
    WHERE,      // :where()
    AND,        // a.b
    COLUMN,     // ||
};

static Selector _parseAttributeSelector(Slice<Css::Sst> content) {
    auto caze = AttributeSelector::INSENSITIVE;
    Str name = "";
    String value = ""s;
    auto match = AttributeSelector::PRESENT;

    usize step = 0;
    Cursor<Css::Sst> cur = content;

    while (not cur.ended() and cur->token.data != "]"s) {
        if (cur.skip(Css::Token::WHITESPACE))
            continue;

        switch (step) {
        case 0:
            name = cur->token.data;
            step++;
            break;
        case 1:
            if (cur->token.data != "="s) {
                auto attrSelectorCase = cur.next().token.data;

                if (cur.ended())
                    break;

                if (not(cur.peek() == (Css::Token::delim("="))))
                    break;

                if (attrSelectorCase == "~") {
                    match = AttributeSelector::CONTAINS;
                } else if (attrSelectorCase == "|") {
                    match = AttributeSelector::HYPHENATED;
                } else if (attrSelectorCase == "^") {
                    match = AttributeSelector::STR_START_WITH;
                } else if (attrSelectorCase == "$") {
                    match = AttributeSelector::STR_END_WITH;
                } else if (attrSelectorCase == "*") {
                    match = AttributeSelector::STR_CONTAIN;
                } else {
                    break;
                }
            } else {
                match = AttributeSelector::EXACT;
            }
            step++;
            break;
        case 2:
            value = parseValue<String>(cur).unwrapOr(""s);
            step++;
            break;
        case 3:
            if (cur->token.data == "s") {
                caze = AttributeSelector::SENSITIVE;
            }
            break;
        }

        if (not cur.ended())
            cur.next();
    }

    return AttributeSelector{
        name,
        caze,
        match,
        value,
    };
}

// consume an Op Code
static OpCode _peekOpCode(Cursor<Css::Sst>& cur) {
    if (cur.ended()) {
        return OpCode::NOP;
    }
    if (*cur != Css::Sst::TOKEN)
        return OpCode::AND;

    switch (cur->token.type) {
    case Css::Token::COMMA:
        cur.next();
        return OpCode::OR;

    case Css::Token::WHITESPACE:
        cur.next();
        // a white space could be an operator or be ignored if followed by another op
        if (cur.ended())
            return OpCode::NOP;

        if (
            cur.peek() == Css::Token::IDENT or
            cur.peek() == Css::Token::HASH or
            cur.peek().token.data == "." or
            cur.peek().token.data == "*" or
            cur.peek().token.data == ":"
        ) {
            return OpCode::DESCENDANT;
        } else {
            auto op = _peekOpCode(cur);

            if (cur.rem() > 1 and cur.peek(1).token == Css::Token::WHITESPACE) {
                if (cur.ended())
                    return OpCode::NOP;
                cur.next();
            }

            return op;
        }

    case Css::Token::DELIM:
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

    case Css::Token::COLON:
    default:
        return OpCode::AND;
    }
}

static Res<Selector> _parseInfixExpr(Selector lhs, Cursor<Css::Sst>& cur, OpCode opCode = OpCode::NOP);

// consume a selector element (everything  that has a lesser priority than the current OP)
static Res<Selector> _parseSelectorElement(Cursor<Css::Sst>& cur, OpCode currentOp) {
    if (cur.ended()) {
        logErrorIf(DEBUG_SELECTORS, "ERROR : unterminated selector");
        return Error::invalidData("unterminated selector");
    }
    Selector val;

    if (*cur == Css::Sst::TOKEN) {
        switch (cur->token.type) {
        case Css::Token::WHITESPACE:
            cur.next();
            return _parseSelectorElement(cur, currentOp);
        case Css::Token::HASH:
            val = IdSelector{next(cur->token.data, 1)};
            break;
        case Css::Token::IDENT:
            val = TypeSelector{TagName::make(cur->token.data, Vaev::HTML)};
            break;
        case Css::Token::DELIM:
            if (cur->token.data == ".") {
                cur.next();
                val = ClassSelector{cur->token.data};
            } else if (cur->token.data == "*") {
                val = UniversalSelector{};
            }
            break;
        case Css::Token::COLON:
            cur.next();
            if (cur->token.type == Css::Token::COLON) {
                cur.next();
                if (cur.ended()) {
                    logErrorIf(DEBUG_SELECTORS, "ERROR : unterminated selector");
                    return Error::invalidData("unterminated selector");
                }
            }

            if (cur->prefix == Css::Token::function("not(")) {
                Cursor<Css::Sst> c = cur->content;
                // consume a whole selector not a single one
                val = Selector::not_(try$(Selector::parse(c)));
            } else {
                val = Pseudo::make(cur->token.data);
            }
            break;
        default:
            val = ClassSelector{cur->token.data};
            break;
        }
    } else if (cur->type == Css::Sst::BLOCK) {
        val = _parseAttributeSelector(cur->content);
    } else {
        return Error::invalidData("unexped sst node");
    }

    cur.next();
    if (not cur.ended()) {
        Cursor rb = cur;
        OpCode nextOpCode = _peekOpCode(cur);
        if (nextOpCode > currentOp) {
            val = try$(_parseInfixExpr(val, cur, nextOpCode));
        } else {
            cur = rb;
        }
    }
    return Ok(val);
}

static Res<Selector> _parseNfixExpr(Selector lhs, OpCode op, Cursor<Css::Sst>& cur) {
    Vec<Selector> selectors = {
        lhs,
        try$(_parseSelectorElement(cur, op)),
    };
    // all the selectors between the op eg : a,b.B,c -> [a,b.B,c]

    while (not cur.ended()) {
        Cursor<Css::Sst> rollBack = cur;

        OpCode nextOpCode = _peekOpCode(cur);

        if (nextOpCode == OpCode::NOP) {
            break;
        } else if (nextOpCode == op) {
            // adding the selector to the nfix
            selectors.pushBack(try$(_parseSelectorElement(cur, op)));
        } else if (nextOpCode == OpCode::COLUMN or nextOpCode == OpCode::OR or nextOpCode == OpCode::AND) {
            // parse new nfix
            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }
            last(selectors) = try$(_parseNfixExpr(last(selectors), nextOpCode, cur));
        } else {
            // parse new infix if the next op is more important

            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }

            if (not(cur.rem() == 2 and cur.peek(1) == Css::Token::WHITESPACE)) [[likely]] {
                last(selectors) = try$(_parseInfixExpr(last(selectors), cur, nextOpCode));

                // auto const lhs = _parseSelectorElement(cur, op);
                // selectors.pushBack(_parseInfixExpr(lhs, cur, nextOpCode));
            } else {
                last(selectors) = try$(_parseInfixExpr(last(selectors), cur, nextOpCode));
                cur.next();
            }
        }
    }

    switch (op) {
    case OpCode::AND:
        return Ok(Selector::and_(selectors));

    case OpCode::OR:
        return Ok(Selector::or_(selectors));

    default:
        return Ok(Selector::and_(selectors));
    }
}

static Res<Selector> _parseInfixExpr(Selector lhs, Cursor<Css::Sst>& cur, OpCode opCode) {
    if (opCode == OpCode::NOP)
        opCode = _peekOpCode(cur);

    switch (opCode) {
    case OpCode::NOP:
        return Ok(lhs);

    case OpCode::DESCENDANT:
        return Ok(Selector::descendant(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::CHILD:
        return Ok(Selector::child(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::ADJACENT:
        return Ok(Selector::adjacent(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::SUBSEQUENT:
        return Ok(Selector::subsequent(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::NOT:
        return Ok(Selector::not_(try$(_parseSelectorElement(cur, opCode))));

    case OpCode::WHERE:
        return Ok(Selector::where(try$(_parseSelectorElement(cur, opCode))));

    case OpCode::COLUMN:
    case OpCode::OR:
    case OpCode::AND:
        return _parseNfixExpr(lhs, opCode, cur);
    }
}

Res<Selector> Selector::parse(Cursor<Css::Sst>& c) {
    if (not c)
        return Error::invalidData("expected selector");

    logDebugIf(DEBUG_SELECTORS, "PARSING SELECTOR : {}", c);
    Selector currentSelector = try$(_parseSelectorElement(c, OpCode::NOP));

    while (not c.ended()) {
        currentSelector = try$(_parseInfixExpr(currentSelector, c));
    }
    return Ok(currentSelector);
}

Res<Selector> Selector::parse(Io::SScan& s) {
    Css::Lexer lex = s;
    auto val = consumeSelector(lex);
    Cursor<Css::Sst> c{val};
    return parse(c);
};

Res<Selector> Selector::parse(Str input) {
    Io::SScan s{input};
    return parse(s);
};
} // namespace Vaev::Style
