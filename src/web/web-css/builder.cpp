#include "builder.h"
#include <karm-logger/logger.h>

namespace Web::Css {

enum struct OpCode {
    DESCENDANT,
    CHILD,
    ADJACENT,
    SUBSEQUENT,
    NOT,
    WHERE,
    AND,
    COLUMN,
    OR
};

static Res<Style::Selector> parseSelectorElement(Vec<Sst> const &prefix, usize &i) {
    logDebug("new selector found {}", prefix[i].token);

    switch (prefix[i].token.type) {
    case Token::HASH:
        return Ok(Style::IdSelector{next(prefix[i].token.data, 1)});
    case Token::IDENT:
        return Ok(Style::TypeSelector{TagName::make(prefix[i].token.data, Web::HTML)});
    case Token::DELIM:
        if (prefix[i].token.data == ".") {
            if (i >= prefix.len()) {
                return Error();
            }
            i++;
            logDebug("new selector found {}", prefix[i].token);
            return Ok(Style::ClassSelector{prefix[i].token.data});
        } else if (prefix[i].token.data == "*") {
            return Ok(Style::UniversalSelector{});
        }

    default:
        return Ok(Style::ClassSelector{prefix[i].token.data});
    }
}

static Res<Style::Selector> parseSelector(auto content) {
    usize i = 0;
    Style::Selector currentSelector = try$(parseSelectorElement(content, i));
    i++;
    return parseInfixExpr(currentSelector, content, i);
}

static Res<OpCode> sstNode2OpCode(Vec<Sst> const &content, usize &i) {
    switch (content[i].token.type) {
    case Token::Type::COMMA:
        return Ok(OpCode::OR);
    case Token::Type::WHITESPACE:
        if (i >= content.len()) {
            return Error();
        }
        if (content[i + 1].token.type != Token::Type::IDENT && content[i + 1].token.type != Token::Type::HASH && content[i + 1].token.data != "*") {
            return Ok(OpCode::DESCENDANT);
        } else {
            i++;
            auto op = sstNode2OpCode(content, i);
            if (i >= content.len()) {
                return Error();
            }
            if (content[i + 1].token.type == Token::Type::WHITESPACE) {
                i++;
            }
            return op;
        }
    default:
        return Ok(OpCode::AND);
    }
}

static Res<Style::Selector> parseInfixExpr(auto lhs, auto content, usize &i) {
    OpCode opCode = try$(sstNode2OpCode(content, i));
    logDebug("INFIX FOUND AFTER {} with OP {}", lhs, opCode);

    // parse OP
    switch (opCode) {
    case OpCode::DESCENDANT:
        return Ok(parseInfixExpr(Style::Selector::descendant({lhs, parseExpr(content)}), content, i));
    case OpCode::CHILD:
        return Ok(Style::Selector::child({lhs, parseSelector(content)}));
    case OpCode::ADJACENT:
        return Ok(Style::Selector::adjacent({lhs, parseSelector(content)}));
    case OpCode::SUBSEQUENT:
        return Ok(Style::Selector::subsequent({lhs, parseSelector(content)}));
    case OpCode::NOT:
        return Ok(Style::Selector::not_({lhs, parseSelector(content)}));
    case OpCode::WHERE:
        return Ok(Style::Selector::where({lhs, parseSelector(content)}));
    case OpCode::AND:
        return Ok(Style::Selector::and_(Vec<Style::Selector>{
            lhs, parseNfixExpr(lhs, content, i)
        }));
    case OpCode::COLUMN:
    case OpCode::OR:
        break;
    }
}

static Res<Style::Selector> parseNfixExpr(auto lhs, Vec<Sst> const &, usize &) {
    logDebug("NFIX FOUND AFTER {}", lhs);
}

static Res<Style::Rule> getRuleObject(auto prefix) {
    switch (prefix[0].type) {
    case Sst::RULE:
    case Sst::FUNC:
    case Sst::DECL:
    case Sst::LIST:
    case Sst::BLOCK:
        return Error();
    case Sst::TOKEN:
        auto tok = prefix[0].token;
        switch (tok.type) {
        case Token::AT_KEYWORD: {
            if (tok.data == "@font-face") {
                Style::FontFaceRule parsed;
                return Ok(parsed);
            } else if (tok.data == "@suports") {
                // TODO parseInternals
                Style::StyleRule parsed = Style::StyleRule(try$(parseSelector(prefix)));
                return Ok(parsed);
            } else if (tok.data == "@media") {
                Style::MediaQuery query = Style::MediaQuery::combineOr(
                    Style::TypeFeature{MediaType::SCREEN},
                    Style::WidthFeature::min(Px{1920})
                );
                Style::MediaRule parsed(query);
                return Ok(parsed);
            }
        }
        default: {
            Style::StyleRule parsed = Style::StyleRule(try$(parseSelector(prefix)));
            // parsed.selector = try$(parseSelector());
            return Ok(parsed);
        }
        }
    }
}

static Res<Style::Rule> parseQualifiedRule(Sst rule) {
    auto &prefix = rule.prefix.unwrap()->content;

    auto parsed = try$(getRuleObject(prefix));

    auto block = rule.content[0].content;
    bool parsingContent = false;
    for (usize i = 0; i < block.len(); i++) {
        switch (block[i].type) {
        case Sst::RULE:
        case Sst::DECL:
        case Sst::LIST:
        case Sst::BLOCK:
            break;
        case Sst::TOKEN:
            if (not parsingContent) {
                if (block[i].token.type != Token::WHITESPACE) {
                    if (block[i].token.type != Token::COLON) {
                        parsed.visit([&](auto &) {
                            // p.declarations.pushBack(Style::StyleDeclaration(block[i].token));
                        });
                    } else {
                        parsingContent = true;
                    }
                }
            } else {
                if (block[i].token.type != Token::SEMICOLON) {
                    parsed.visit([&](auto &) {
                        // last(p.declarations).value.pushBack(block[i].token);
                    });
                } else {
                    parsingContent = false;
                }
            }
            break;
        case Sst::FUNC:
            break;
        }
    }
    return Ok(parsed);
}

// No spec, we take the SST we built and convert it to a usable list of rules
Vec<Style::Rule> parseSST(Sst sst) {
    Vec<Style::Rule> rules;
    for (usize i = 0; i < sst.content.len(); i++) {
        switch (sst.content[i].type) {

        case Sst::_Type::RULE:
            // rules.pushBack(parseQualifiedRule(sst.content[i]).unwrap());
            break;
        case Sst::_Type::FUNC:
        case Sst::_Type::DECL:
        case Sst::_Type::LIST:
        case Sst::_Type::TOKEN:
        case Sst::_Type::BLOCK:
            break;
        }
    }
    return rules;
}

} // namespace Web::Css
