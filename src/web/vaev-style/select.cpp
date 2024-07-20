#include "select.h"

#include "values.h"

namespace Vaev::Style {

// MARK: Selector Specificity ---------------------------------------------------

// https://www.w3.org/TR/selectors-3/#specificity
Spec spec(Selector const &s) {
    return s.visit(Visitor{
        [](Nfix const &n) {
            if (n.type == Nfix::WHERE)
                return Spec::ZERO;

            Spec sum = Spec::ZERO;
            for (auto &inner : n.inners)
                sum = sum + spec(inner);
            return sum;
        },
        [](Infix const &i) {
            return spec(*i.lhs) + spec(*i.rhs);
        },
        [](UniversalSelector const &) {
            return Spec::ZERO;
        },
        [](EmptySelector const &) {
            return Spec::ZERO;
        },
        [](IdSelector const &) {
            return Spec::A;
        },
        [](TypeSelector const &) {
            return Spec::C;
        },
        [](ClassSelector const &) {
            return Spec::B;
        },
        [](Pseudo const &) {
            return Spec::ZERO;
        },
        [](AttributeSelector const &) {
            return Spec::ZERO;
        },
        [](auto const &s) {
            logWarn("unimplemented selector: {}", s);
            return Spec::ZERO;
        }
    });
}

// MARK: Selector Matching -----------------------------------------------------

// https://www.w3.org/TR/selectors-4/#descendant-combinators
static bool _matchDescendant(Selector const &s, Dom::Element const &e) {
    Dom::Node const *curr = &e;
    while (curr->hasParent()) {
        auto &parent = curr->parentNode();
        if (auto *el = parent.is<Dom::Element>())
            if (s.match(*el))
                return true;
        curr = &parent;
    }
    return false;
}

// https://www.w3.org/TR/selectors-4/#child-combinators
static bool _matchChild(Selector const &s, Dom::Element const &e) {
    if (not e.hasParent())
        return false;

    auto &parent = e.parentNode();
    if (auto *el = parent.is<Dom::Element>())
        return s.match(*el);
    return false;
}

// https://www.w3.org/TR/selectors-4/#adjacent-sibling-combinators
static bool _matchAdjacent(Selector const &s, Dom::Element const &e) {
    if (not e.hasPreviousSibling())
        return false;

    auto prev = e.previousSibling();
    if (auto *el = prev.is<Dom::Element>())
        return s.match(*el);
    return false;
}

// https://www.w3.org/TR/selectors-4/#general-sibling-combinators
static bool _matchSubsequent(Selector const &s, Dom::Element const &e) {
    Dom::Node const *curr = &e;
    while (curr->hasPreviousSibling()) {
        auto prev = curr->previousSibling();
        if (auto *el = prev.is<Dom::Element>())
            if (s.match(*el))
                return true;
        curr = &prev.unwrap();
    }
    return false;
}

static bool _match(Infix const &s, Dom::Element const &e) {
    if (not s.lhs->match(e))
        return false;

    switch (s.type) {
    case Infix::Type::DESCENDANT:
        return _matchDescendant(*s.rhs, e);

    case Infix::Type::CHILD:
        return _matchChild(*s.rhs, e);

    case Infix::Type::ADJACENT:
        return _matchAdjacent(*s.rhs, e);

    case Infix::Type::SUBSEQUENT:
        return _matchSubsequent(*s.rhs, e);

    default:
        logWarn("unimplemented selector: {}", s);
        return false;
    }
}

static bool _match(Nfix const &s, Dom::Element const &el) {
    switch (s.type) {
    case Nfix::AND:
        for (auto &inner : s.inners)
            if (not inner.match(el))
                return false;
        return true;

    // 4.1. Selector Lists
    // https://www.w3.org/TR/selectors-4/#grouping
    // and
    // 4.2. The Matches-Any Pseudo-class: :is()
    // https://www.w3.org/TR/selectors-4/#matchess
    case Nfix::OR:
        for (auto &inner : s.inners)
            if (inner.match(el))
                return true;
        return false;

    case Nfix::NOT:
        return not s.inners[0].match(el);

    case Nfix::WHERE:
        return not s.inners[0].match(el);

    default:
        logWarn("unimplemented selector: {}", s);
        return false;
    }
}

// 5.1. Type (tag name) selector
// https://www.w3.org/TR/selectors-4/#type
static bool _match(TypeSelector const &s, Dom::Element const &el) {
    return el.tagName == s.type;
}

static bool _match(IdSelector const &s, Dom::Element const &el) {
    return el.id() == s.id;
}

static bool _match(ClassSelector const &s, Dom::Element const &el) {
    return el.classList.contains(s.class_);
}

// 5.2. Universal selector
// https://www.w3.org/TR/selectors-4/#the-universal-selector
static bool _match(UniversalSelector const &, Dom::Element const &) {
    return true;
}

// MARK: Selector --------------------------------------------------------------

bool Selector::match(Dom::Element const &el) const {
    return visit(
        [&](auto const &s) {
            if constexpr (requires { _match(s, el); })
                return _match(s, el);

            logWarn("unimplemented selector: {}", s);
            return false;
        }
    );
}

// MARK: Parser ----------------------------------------------------------------

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

static Selector _parseAttributeSelector(Slice<Css::Sst> content) {
    auto caze = AttributeSelector::INSENSITIVE;
    Str name = "";
    String value = ""s;
    auto match = AttributeSelector::Match{AttributeSelector::PRESENT};

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
                if (cur.ended() or cur.peek(1).token.data != "="s) {
                    break;
                }

                if (cur->token.data == "~") {
                    match = AttributeSelector::Match{AttributeSelector::CONTAINS};
                } else if (cur->token.data == "|") {
                    match = AttributeSelector::Match{AttributeSelector::HYPHENATED};
                } else if (cur->token.data == "^") {
                    match = AttributeSelector::Match{AttributeSelector::STR_START_WITH};
                } else if (cur->token.data == "$") {
                    match = AttributeSelector::Match{AttributeSelector::STR_END_WITH};
                } else if (cur->token.data == "*") {
                    match = AttributeSelector::Match{AttributeSelector::STR_CONTAIN};
                } else {
                    break;
                }
                cur.next();
            } else {
                match = AttributeSelector::Match{AttributeSelector::EXACT};
            }
            step++;
            break;
        case 2:
            value = parseValue<String>(cur).unwrap();
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

    return AttributeSelector{name, caze, match, value};
}

static Selector _parseSelectorElement(Cursor<Css::Sst> &cur) {
    Selector val;

    if (*cur == Css::Sst::TOKEN) {
        switch (cur->token.type) {
        case Css::Token::WHITESPACE:
            if (cur.ended()) {
                // logError("ERROR : unterminated selector");
                return EmptySelector{};
            }
            cur.next();
            return _parseSelectorElement(cur);
        case Css::Token::HASH:
            if (cur.ended()) {
                // logError("ERROR : unterminated selector");
                return EmptySelector{};
            }
            val = IdSelector{next(cur->token.data, 1)};
            break;
        case Css::Token::IDENT:
            val = TypeSelector{TagName::make(cur->token.data, Vaev::HTML)};
            break;
        case Css::Token::DELIM:
            if (cur->token.data == ".") {
                cur.next();
                if (cur.ended()) {
                    logError("ERROR : unterminated selector");
                    return EmptySelector{};
                }
                val = ClassSelector{cur->token.data};
            } else if (cur->token.data == "*") {
                val = UniversalSelector{};
            }
            break;
        case Css::Token::COLON:
            cur.next();
            if (cur.ended()) {
                logError("ERROR : unterminated selector");
                return EmptySelector{};
            }
            if (cur->token.type == Css::Token::COLON) {
                cur.next();
                if (cur.ended()) {
                    logError("ERROR : unterminated selector");
                    return EmptySelector{};
                }
            }
            val = Pseudo{Pseudo::make(cur->token.data)};
            break;
        default:
            val = ClassSelector{cur->token.data};
            break;
        }
    } else if (cur->type == Css::Sst::BLOCK) {
        val = _parseAttributeSelector(cur->content);
    } else {
        return EmptySelector{};
    }

    cur.next();
    return val;
}

static OpCode _peekOpCode(Cursor<Css::Sst> &cur) {
    if (*cur != Css::Sst::TOKEN)
        return OpCode::AND;

    switch (cur->token.type) {
    case Css::Token::COMMA:
        if (cur.ended()) {
            return OpCode::NOP;
        }
        cur.next();
        return OpCode::OR;

    case Css::Token::WHITESPACE:
        cur.next();
        // a white space could be an operator or be ignored if followed by another op
        if (cur.ended()) {
            return OpCode::NOP;
        }

        if (
            cur.peek() == Css::Token::IDENT or
            cur.peek() == Css::Token::HASH or
            cur.peek().token.data == "." or
            cur.peek().token.data == "*"
        ) {
            return OpCode::DESCENDANT;
        } else {
            auto op = _peekOpCode(cur);

            if (cur.peek(1).token.type == Css::Token::WHITESPACE) {
                if (cur.ended()) {
                    return OpCode::NOP;
                }
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
        if (cur.ended())
            return OpCode::NOP;

    default:
        return OpCode::AND;
    }
}

static Selector _parseInfixExpr(Selector lhs, Cursor<Css::Sst> &cur, OpCode opCode = OpCode::NOP);

static Selector _parseNfixExpr(Selector lhs, OpCode op, Cursor<Css::Sst> &cur) {
    Vec<Selector> selectors = {lhs, _parseSelectorElement(cur)};

    while (not cur.ended()) {
        Cursor<Css::Sst> rollBack = cur;

        OpCode nextOpCode = _peekOpCode(cur);

        if (nextOpCode == OpCode::NOP) {
            break;
        }

        if (nextOpCode == op) {
            // adding the selector to the nfix
            selectors.pushBack(_parseSelectorElement(cur));
        } else if (nextOpCode == OpCode::COLUMN or nextOpCode == OpCode::OR or nextOpCode == OpCode::AND) {
            // parse new nfix

            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }

            last(selectors) = _parseNfixExpr(last(selectors), nextOpCode, cur);
        } else {
            // parse new infix
            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }

            selectors.pushBack(_parseInfixExpr(_parseSelectorElement(cur), cur, nextOpCode));
        }
    }

    switch (op) {
    case OpCode::AND:
        return Selector::and_(selectors);

    case OpCode::OR:
        return Selector::or_(selectors);

    default:
        return Selector::and_(selectors);
    }
}

static Selector _parseInfixExpr(Selector lhs, Cursor<Css::Sst> &cur, OpCode opCode) {
    if (opCode == OpCode::NOP)
        opCode = _peekOpCode(cur);

    switch (opCode) {
    case OpCode::NOP:
        return lhs;

    case OpCode::DESCENDANT:
        return Selector::descendant(lhs, _parseSelectorElement(cur));

    case OpCode::CHILD:
        return Selector::child(lhs, _parseSelectorElement(cur));

    case OpCode::ADJACENT:
        return Selector::adjacent(lhs, _parseSelectorElement(cur));

    case OpCode::SUBSEQUENT:
        return Selector::subsequent(lhs, _parseSelectorElement(cur));

    case OpCode::NOT:
        return Selector::not_(_parseSelectorElement(cur));

    case OpCode::WHERE:
        return Selector::where(_parseSelectorElement(cur));

    case OpCode::COLUMN:
    case OpCode::OR:
    case OpCode::AND:
        return _parseNfixExpr(lhs, opCode, cur);
    }
}

Selector Selector::parse(Cursor<Css::Sst> &c) {
    if (!c) {
        logWarn("empty selector");
        return EmptySelector{};
    }

    Selector currentSelector = _parseSelectorElement(c);

    while (not c.ended()) {
        currentSelector = _parseInfixExpr(currentSelector, c);
    }
    return currentSelector;
}

Selector Selector::parse(Io::SScan &s) {
    Css::Lexer lex = s;
    auto val = consumeSelector(lex);
    Cursor<Css::Sst> c{val};
    return parse(c);
};

Selector Selector::parse(Str input) {
    Io::SScan s{input};
    return parse(s);
};

} // namespace Vaev::Style
