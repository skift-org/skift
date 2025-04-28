#include "page.h"

#include "decls.h"

namespace Vaev::Style {

static bool DEBUG_PAGE = false;

// MARK: Page Selector ----------------------------------------------------------

bool PageSelector::match(Page const& page) const {
    if (name and page.name != name)
        return false;

    for (auto const& pseudo : pseudos) {
        switch (pseudo) {
        case PagePseudo::FIRST:
            if (not page.number)
                return false;
            break;
        case PagePseudo::BLANK:
            if (page.blank)
                return false;
            break;
        case PagePseudo::LEFT:
            if (page.number % 2 == 0)
                return false;
            break;
        case PagePseudo::RIGHT:
            if (page.number % 2 == 1)
                return false;
            break;
        }
    }

    return true;
}

void PageSelector::repr(Io::Emit& e) const {
    e("({} pseudos: {})", name, pseudos);
}

static Res<PagePseudo> _parsePagePseudo(Cursor<Css::Sst>& c) {
    auto rollback = c.rollbackPoint();

    if (not c.skip(Css::Token::COLON))
        return Error::invalidData("expected ':'");

    eatWhitespace(c);

    if (c.peek() != Css::Token::IDENT)
        return Error::invalidData("expected page pseudo");

    rollback.disarm();
    auto name = c.next().token.data;
    if (name == "first") {
        return Ok(PagePseudo::FIRST);
    } else if (name == "blank") {
        return Ok(PagePseudo::BLANK);
    } else if (name == "left") {
        return Ok(PagePseudo::LEFT);
    } else if (name == "right") {
        return Ok(PagePseudo::RIGHT);
    } else {
        rollback.arm();
        return Error::invalidData("unknown page pseudo");
    }
}

PageSelector PageSelector::parse(Cursor<Css::Sst>& c) {
    PageSelector res;

    if (c.peek() == Css::Token::IDENT) {
        res.name = c.next().token.data;
    }

    eatWhitespace(c);
    while (auto it = _parsePagePseudo(c)) {
        res.pseudos.pushBack(it.take());
        eatWhitespace(c);
    }

    return res;
}

Vec<PageSelector> PageSelector::parseList(Cursor<Css::Sst>& c) {
    Vec<PageSelector> res;

    eatWhitespace(c);
    while (not c.ended()) {
        res.pushBack(parse(c));
        eatWhitespace(c);
    }

    return res;
}

// MARK: Page Margin Rule ------------------------------------------------------

void PageAreaRule::apply(ComputedStyle& c) const {
    for (auto const& prop : props) {
        prop.apply(c, c);
    }
}

void PageAreaRule::repr(Io::Emit& e) const {
    e("(page-margin-rule\nmargin: {}\nprops: {})", area, props);
}

static Opt<PageArea> _parsePageArea(Css::Token tok) {
    Str name = next(tok.data);

#define ITER(ID, NAME) \
    if (name == NAME)  \
        return PageArea::ID;
    FOREACH_PAGE_AREA(ITER)
#undef ITER

    logWarn("unknown page area: {}", name);

    return NONE;
}

Opt<PageAreaRule> PageAreaRule::parse(Css::Sst const& sst) {
    PageAreaRule res;

    res.area = try$(_parsePageArea(sst.token));

    for (auto const& item : sst.content) {
        if (item == Css::Sst::DECL) {
            auto prop = parseDeclaration<StyleProp>(item);
            if (prop)
                res.props.pushBack(prop.take());
        } else {
            logWarnIf(DEBUG_PAGE, "unexpected item in style rule: {}", item);
        }
    }

    return res;
}

// MARK: Page Rule -------------------------------------------------------------

bool PageRule::match(Page const& page) const {
    if (selectors.len() == 0)
        return true;

    for (auto& s : selectors) {
        if (s.match(page))
            return true;
    }

    return false;
}

void PageRule::apply(PageComputedStyle& c) const {
    for (auto const& prop : props) {
        prop.apply(*c.style, *c.style);
    }

    for (auto const& area : areas) {
        auto computed = c.area(area.area);
        area.apply(*computed);
    }
}

void PageRule::repr(Io::Emit& e) const {
    e("(page-rule\nselectors: {}\nprops: {}\nmargins: {})", selectors, props, areas);
}

PageRule PageRule::parse(Css::Sst const& sst) {
    if (sst != Css::Sst::RULE)
        panic("expected rule");

    if (sst.prefix != Css::Sst::LIST)
        panic("expected list");

    PageRule res;

    // Parse the selector
    auto& prefix = sst.prefix.unwrap();
    Cursor<Css::Sst> prefixContent = prefix->content;
    res.selectors = PageSelector::parseList(prefixContent);

    // Parse the properties.
    for (auto const& item : sst.content) {
        if (item == Css::Sst::DECL) {
            auto prop = parseDeclaration<StyleProp>(item);
            if (prop)
                res.props.pushBack(prop.take());
        } else if (item == Css::Sst::RULE and
                   item.token == Css::Token::AT_KEYWORD) {
            auto rule = PageAreaRule::parse(item);
            if (rule)
                res.areas.pushBack(*rule);
        } else {
            logWarnIf(DEBUG_PAGE, "unexpected item in style rule: {}", item);
        }
    }

    return res;
}

} // namespace Vaev::Style
