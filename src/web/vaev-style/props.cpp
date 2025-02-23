#include <vaev-style/decls.h>

#include "props.h"

namespace Vaev::Style {

static bool DEBUG_PROPS = false;

// MARK: DeferredProp ----------------------------------------------------------

bool DeferredProp::_expandVariable(Cursor<Css::Sst>& c, Map<String, Css::Content> const& env, Css::Content& out) {
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
        Cursor<Css::Sst> varContent = *ref;
        _expandContent(varContent, env, out);
        return true;
    }
    content.next();

    eatWhitespace(content);

    if (not content.skip(Css::Token::COMMA))
        return true;

    _expandContent(content, env, out);
    return true;
}

bool DeferredProp::_expandFunction(Cursor<Css::Sst>& c, Map<String, Css::Content> const& env, Css::Content& out) {
    if (c->type != Css::Sst::FUNC)
        return false;

    auto& func = out.emplaceBack(Css::Sst::FUNC);
    func.prefix = c->prefix;
    Cursor<Css::Sst> content = c->content;
    _expandContent(content, env, func.content);

    return true;
}

void DeferredProp::_expandContent(Cursor<Css::Sst>& c, Map<String, Css::Content> const& env, Css::Content& out) {
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

void DeferredProp::apply(Computed const& parent, Computed& c) const {
    Css::Sst decl{Css::Sst::DECL};
    decl.token = Css::Token::ident(propName);
    Cursor<Css::Sst> cursor = value;
    _expandContent(cursor, *c.variables, decl.content);

    // Parse the expanded content
    Res<StyleProp> computed = parseDeclaration<StyleProp>(decl, false);
    if (not computed) {
        logWarnIf(DEBUG_PROPS, "failed to parse declaration: {}: {}", decl, computed);
    } else {
        computed.unwrap().apply(parent, c);
    }
}

// MARK: DefaultedProp ---------------------------------------------------------

void DefaultedProp::apply(Computed const& parent, Computed& c) const {
    if (value == Default::INITIAL) {
        StyleProp::any([&]<typename T>(Meta::Type<T>) {
            if (T::name() != propName)
                return false;
            if constexpr (requires { T::initial(); })
                StyleProp{T{T::initial()}}.apply(parent, c);
            return true;
        });
    } else if (value == Default::INHERIT) {
        StyleProp::any([&]<typename T>(Meta::Type<T>) {
            if (T::name() != propName)
                return false;
            if constexpr (requires { T::load(parent); })
                StyleProp{T{T::load(parent)}}.apply(parent, c);
            return true;
        });
    } else if (value == Default::UNSET) {
        StyleProp::any([&]<typename T>(Meta::Type<T>) {
            if (T::name() != propName)
                return false;
            if constexpr (requires { T::inherit;  T::load(parent); })
                StyleProp{T{T::load(parent)}}.apply(parent, c);
            else if constexpr (requires { T::initial(); })
                StyleProp{T{T::initial()}}.apply(parent, c);
            return true;
        });
    } else {
        logDebug("defaulted: unsupported value '{}'", value);
    }
}

void DefaultedProp::repr(Io::Emit& e) const {
    e("(Defaulted {#} = {})", propName, value);
}

// MARK: Style Property  -------------------------------------------------------

Str StyleProp::name() const {
    return visit([](auto const& p) {
        return p.name();
    });
}

void StyleProp::inherit(Computed const& parent, Computed& child) const {
    visit([&](auto const& p) {
        if constexpr (requires { p.inherit(parent, child); })
            p.inherit(parent, child);
    });
}

void StyleProp::apply(Computed const& parent, Computed& c) const {
    visit([&](auto const& p) {
        if constexpr (requires { p.apply(c); })
            p.apply(c);

        if constexpr (requires { p.apply(parent, c); })
            p.apply(parent, c);
    });
}

void StyleProp::repr(Io::Emit& e) const {
    e("({}", name());
    visit([&](auto const& p) {
        e(" {#}", p.value);
        if (important == Important::YES)
            e(" !important");
    });
    e(")");
}

} // namespace Vaev::Style
