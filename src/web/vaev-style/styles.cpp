#include <vaev-style/decls.h>

#include "styles.h"

namespace Vaev::Style {

// MARK: DeferredProp ----------------------------------------------------------

bool DeferredProp::_expandVariable(Cursor<Css::Sst> &c, Map<String, Css::Content> const &env, Css::Content &out) {
    if (not(c->type == Css::Sst::FUNC and
            c->prefix == Css::Token::function("var(")))
        return false;

    Cursor<Css::Sst> content = c->content;

    eatWhitespace(content);
    if (content.ended())
        return true;

    if (content.peek() != Css::Token::IDENT)
        return true;

    Str varName = content->token.data;
    if (auto ref = env.access(varName)) {
        out.pushBack(*ref);
        return true;
    }
    content.next();

    eatWhitespace(content);

    if (not content.skip(Css::Token::COMMA))
        return true;

    _expandContent(content, env, out);
    return true;
}

bool DeferredProp::_expandFunction(Cursor<Css::Sst> &c, Map<String, Css::Content> const &env, Css::Content &out) {
    if (c->type != Css::Sst::FUNC)
        return false;

    auto &func = out.emplaceBack(Css::Sst::FUNC);
    Cursor<Css::Sst> content = c->content;
    _expandContent(content, env, func.content);

    return true;
}

void DeferredProp::_expandContent(Cursor<Css::Sst> &c, Map<String, Css::Content> const &env, Css::Content &out) {
    // NOTE: Hint that we will add all the remaining elements
    out.ensure(out.len() + c.rem());

    while (not c.ended()) {
        if (not _expandVariable(c, env, out) and
            not _expandFunction(c, env, out)) {
            out.pushBack(*c);
        }

        c.next();
    }
}

void DeferredProp::apply(Computed &c) const {
    Css::Sst decl{Css::Sst::DECL};
    decl.token = Css::Token::ident(propName);
    Cursor<Css::Sst> cursor = value;
    _expandContent(cursor, c.variables.cow(), decl.content);

    // Parse the expanded content
    Res<StyleProp> computed = parseDeclaration<StyleProp>(decl, false);
    if (not computed) {
        logWarn("failed to parse declaration: {}", computed);
    } else {
        computed.unwrap().apply(c);
    }
}

} // namespace Vaev::Style
