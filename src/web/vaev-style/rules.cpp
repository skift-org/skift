#include "rules.h"
#include "selector-matching.h"
#include "decls.h"

namespace Vaev::Style {

static bool DEBUG_RULE = false;

// MARK: StyleRule -------------------------------------------------------------

Opt<Spec> StyleRule::matchWithSpecificity(Markup::Element const& el) const {
    return matchSelectorWithSpecificity(selector,el);
}

void StyleRule::repr(Io::Emit& e) const {
    e("(style-rule");
    e.indent();
    e("\nselector: {}", selector);
    if (props) {
        e.newline();
        e("props: [");
        e.indentNewline();
        for (auto const& prop : props) {
            e("{}\n", prop);
        }
        e.deindent();
        e("]\n");
    }
    e.deindent();
    e(")");
}

StyleRule StyleRule::parse(Css::Sst const& sst, Origin origin) {
    if (sst != Css::Sst::RULE)
        panic("expected rule");

    if (sst.prefix != Css::Sst::LIST)
        panic("expected list");

    Style::StyleRule res;

    // Parse the selector.
    auto& prefix = sst.prefix.unwrap();
    Cursor<Css::Sst> prefixContent = prefix->content;
    auto maybeSelector = Selector::parse(prefixContent);
    if (maybeSelector) {
        res.selector = maybeSelector.take();
    } else {
        logWarn("failed to parse selector: {}: {}", prefix->content, maybeSelector);
        res.selector = EmptySelector{};
    }

    // Parse the properties.
    for (auto const& item : sst.content) {
        if (item == Css::Sst::DECL) {
            auto prop = parseDeclaration<StyleProp>(item);
            if (prop)
                res.props.pushBack(prop.take());
        } else {
            logWarnIf(DEBUG_RULE, "unexpected item in style rule: {}", item);
        }
    }

    res.origin = origin;
    return res;
}

// MARK: ImportRule ------------------------------------------------------------

void ImportRule::repr(Io::Emit& e) const {
    e("(import-rule {})", url);
}

ImportRule ImportRule::parse(Css::Sst const&) {
    return {};
}

// MARK: MediaRule -------------------------------------------------------------

void MediaRule::repr(Io::Emit& e) const {
    e("(media-rule");
    e.indent();
    e("\nmedia: {}", media);
    if (rules) {
        e.newline();
        e("rules: [");
        e.indentNewline();
        for (auto const& rule : rules) {
            e("{}\n", rule);
        }
        e.deindent();
        e("]\n");
    }
}

bool MediaRule::match(Media const& m) const {
    return media.match(m);
}

MediaRule MediaRule::parse(Css::Sst const& sst) {
    if (sst != Css::Sst::RULE)
        panic("expected rule");

    if (sst.prefix != Css::Sst::LIST)
        panic("expected list");

    Style::MediaRule res;

    // Parse the media query.
    auto& prefix = sst.prefix.unwrap();
    Cursor<Css::Sst> prefixContent = prefix->content;
    res.media = parseMediaQuery(prefixContent);

    // Parse the rules.
    for (auto const& item : sst.content) {
        if (item == Css::Sst::RULE) {
            res.rules.pushBack(Rule::parse(item));
        } else {
            logWarn("unexpected item in media rule: {}", item.type);
        }
    }

    return res;
}

// MARK: FontFaceRule ----------------------------------------------------------

void FontFaceRule::repr(Io::Emit& e) const {
    e("(font-face-rule {})", descs);
}

FontFaceRule FontFaceRule::parse(Css::Sst const& sst) {
    return {parseDeclarations<FontDesc>(sst, false)};
}

// MARK: Rule ------------------------------------------------------------------

void Rule::repr(Io::Emit& e) const {
    visit([&](auto const& r) {
        e("{}", r);
    });
}

Rule Rule::parse(Css::Sst const& sst, Origin origin) {
    if (sst != Css::Sst::RULE)
        panic("expected rule");

    auto tok = sst.token;
    if (tok.data == "@media")
        return MediaRule::parse(sst);
    else if (tok.data == "@import")
        return ImportRule::parse(sst);
    else if (tok.data == "@font-face")
        return FontFaceRule::parse(sst);
    else if (tok.data == "@page")
        return PageRule::parse(sst);
    else
        return StyleRule::parse(sst, origin);
}

} // namespace Vaev::Style
