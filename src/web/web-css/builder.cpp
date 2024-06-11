#include "builder.h"

#include "karm-logger/logger.h"

namespace Web::Css {
Style::Select::Selector
static Res<Select::Selector> getSelectorElement(Vec<Sst> const &prefix, usize &i) {
    logDebug("new selector found {}", prefix[i].token);

    switch (prefix[i].token.type) {
    case Token::HASH:
        return Ok(Select::IdSelector{next(prefix[i].token.data, 1)});
    case Token::IDENT:
        return Ok(Select::TypeSelector{TagName::make(prefix[i].token.data, Web::HTML)});
    case Token::DELIM:
        if (prefix[i].token.data == ".") {
            if (i >= prefix.len()) {
                return Error();
            }
            i++;
            logDebug("new selector found {}", prefix[i].token);
            return Ok(Select::ClassSelector{prefix[i].token.data});
        } else if (prefix[i].token.data == "*") {
            return Ok(Select::UniversalSelector{});
        }

    default:
        return Ok(Select::ClassSelector{prefix[i].token.data});
    }
}

static Res<Select::Selector> composeSelectors(Select::Selector sel1, Select::Selector sel2) {
    logDebug("composing {} and {}", sel1, sel2);
    if (sel1.is<Select::Infix>()) {
        return Ok(sel1.selectors);
    } else {
        return Ok(Select::Selector::and_(Vec<Select::Selector>{sel1, sel2}));
    }

    return Ok(sel1);
}

static Res<Select::Selector> parseSelector(auto content) {
    usize i = 0;
    Select::Selector currentSelector = try$(getSelectorElement(content, i));
    i++;
    return parseInfixExpr(currentSelector, content, i);
}

static Select::OpCode sstNode2OpCode(Vec<Sst> const &content, usize &i) {
    switch (content[i].token.type) {
    case Token::Type::COMMA:
        return Select::OpCode::OR;
    case Token::Type::WHITESPACE:
        return Select::OpCode::DESCENDANT;
    case Token::Type::DELIM:
        break;
    default:
        return Select::OpCode::AND;
    }
}

static Res<Select::Selector> parseInfixExpr(auto lhs, auto content, usize &i) {
    Select::OpCode opCode = sstNode2OpCode(content, i);

    // parse OP
    switch (opCode) {
    case Select::OpCode::DESCENDANT:
        return Ok(Select::Selector::descendant({lhs, parseSelector(content)}));
    case Select::OpCode::CHILD:
        return Ok(Select::Selector::child({lhs, parseSelector(content)}));
    case Select::OpCode::ADJACENT:
        return Ok(Select::Selector::adjacent({lhs, parseSelector(content)}));
    case Select::OpCode::SUBSEQUENT:
        return Ok(Select::Selector::subsequent({lhs, parseSelector(content)}));
    case Select::OpCode::NOT:
        return Ok(Select::Selector::not_({lhs, parseSelector(content)}));
    case Select::OpCode::WHERE:
        return Ok(Select::Selector::where({lhs, parseSelector(content)}));
    case Select::OpCode::DESCENDANT:
        return Ok(Select::Selector::descendant({lhs, parseSelector(content)}));
    case Select::OpCode::AND:
        return Ok(Select::Selector::and_(Vec<Select::Selector>{
            lhs, parseNfixExpr(lhs, content, i)
        }));
    case Select::OpCode::CHILD:
    case Select::OpCode::ADJACENT:
    case Select::OpCode::SUBSEQUENT:
    case Select::OpCode::COLUMN:
    case Select::OpCode::OR:
    case Select::OpCode::NOT:
    case Select::OpCode::WHERE:
        break;
    }
}

static Res<Select::Selector> parseNfixExpr(auto lhs, Vec<Sst> const &content, usize &i) {

    while (true) {
        Select::OpCode opCode = sstNode2OpCode(content, i);
        auto rhs = parseSelector(content);

        // parse OP
        switch (opCode) {
        case Select::OpCode::DESCENDANT:
            return Ok(Select::Selector::descendant({lhs, parseSelector(content)}));
        case Select::OpCode::CHILD:
            return Ok(Select::Selector::child({lhs, parseSelector(content)}));
        case Select::OpCode::ADJACENT:
            return Ok(Select::Selector::adjacent({lhs, parseSelector(content)}));
        case Select::OpCode::SUBSEQUENT:
            return Ok(Select::Selector::subsequent({lhs, parseSelector(content)}));
        case Select::OpCode::NOT:
            return Ok(Select::Selector::not_({lhs, parseSelector(content)}));
        case Select::OpCode::WHERE:
            return Ok(Select::Selector::where({lhs, parseSelector(content)}));
        case Select::OpCode::AND:
            lhs.selectors.pushBack(rhs);
            break;
        case Select::OpCode::COLUMN:
            return Ok(Select::Selector::column(Vec<Select::Selector>{
                parseNfixExpr(lhs, content, i)
            }));
        case Select::OpCode::OR:
            return Ok(Select::Selector::or_(Vec<Select::Selector>{
                lhs, parseNfixExpr(lhs, content, i)
            }));
        }
    }
}

static Res<CSSOM::CSSRule> getRuleObject(auto prefix) {
    switch (prefix[0].type) {
    case Sst::QUALIFIED_RULE:
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
                CSSOM::CSSFontFaceRule parsed;
                return Ok(parsed);
            } else if (tok.data == "@suports") {
                CSSOM::CSSSupportsRule parsed;
                return Ok(parsed);
            } else if (tok.data == "@media") {
                Media::Query query = Media::Query::combineOr(
                    Media::TypeFeature{Media::Type::SCREEN},
                    Media::WidthFeature::min(Types::Px{1920})
                );
                CSSOM::CSSMediaRule parsed(query);
                return Ok(parsed);
            }
        }
        default: {
            CSSOM::CSSStyleRule parsed = CSSOM::CSSStyleRule(try$(parseSelector(prefix)));
            // parsed.selector = try$(parseSelector());
            return Ok(parsed);
        }
        }
    }
}

static Res<CSSOM::CSSRule> parseQualifiedRule(Sst rule) {
    auto &prefix = rule.prefix.unwrap()->content;

    auto parsed = try$(getRuleObject(prefix));

    auto block = rule.content[0].content;
    bool parsingContent = false;
    for (usize i = 0; i < block.len(); i++) {
        switch (block[i].type) {
        case Sst::QUALIFIED_RULE:
        case Sst::DECL:
        case Sst::LIST:
        case Sst::BLOCK:
            break;
        case Sst::TOKEN:
            if (not parsingContent) {
                if (block[i].token.type != Token::WHITESPACE) {
                    if (block[i].token.type != Token::COLON) {
                        parsed.visit([&](auto &p) {
                            p.declarations.pushBack(CSSOM::CSSStyleDeclaration(block[i].token));
                        });
                    } else {
                        parsingContent = true;
                    }
                }
            } else {
                if (block[i].token.type != Token::SEMICOLON) {
                    parsed.visit([&](auto &p) {
                        last(p.declarations).value.pushBack(block[i].token);
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
static inline Vec<CSSOM::CSSRule> parseSST(Sst sst) {
    Vec<CSSOM::CSSRule> rules;
    for (usize i = 0; i < sst.content.len(); i++) {
        switch (sst.content[i].type) {

        case Sst::_Type::QUALIFIED_RULE:
            rules.pushBack(parseQualifiedRule(sst.content[i]).unwrap());
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
