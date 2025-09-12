module;

#include <karm-core/macros.h>
#include <karm-logger/logger.h>

export module Vaev.Engine:style.page;

import :dom;
import :style.origin;
import :style.decls;

namespace Vaev::Style {

static bool DEBUG_PAGE = false;

// https://drafts.csswg.org/css-page-3/#margin-at-rules
#define FOREACH_PAGE_AREA(ITER)                      \
    ITER(FOOTNOTE, "footnote")                       \
    ITER(TOP, "-vaev-top")                           \
    ITER(TOP_LEFT_CORNER, "top-left-corner")         \
    ITER(TOP_LEFT, "top-left")                       \
    ITER(TOP_CENTER, "top-center")                   \
    ITER(TOP_RIGHT, "top-right")                     \
    ITER(TOP_RIGHT_CORNER, "top-right-corner")       \
    ITER(RIGHT, "-vaev-right")                       \
    ITER(RIGHT_TOP, "right-top")                     \
    ITER(RIGHT_MIDDLE, "right-middle")               \
    ITER(RIGHT_BOTTOM, "right-bottom")               \
    ITER(BOTTOM, "-vaev-bottom")                     \
    ITER(BOTTOM_RIGHT_CORNER, "bottom-right-corner") \
    ITER(BOTTOM_RIGHT, "bottom-right")               \
    ITER(BOTTOM_CENTER, "bottom-center")             \
    ITER(BOTTOM_LEFT, "bottom-left")                 \
    ITER(BOTTOM_LEFT_CORNER, "bottom-left-corner")   \
    ITER(LEFT, "-vaev-left")                         \
    ITER(LEFT_BOTTOM, "left-bottom")                 \
    ITER(LEFT_MIDDLE, "left-middle")                 \
    ITER(LEFT_TOP, "left-top")

export enum struct PageArea {
#define ITER(ID, ...) ID,
    FOREACH_PAGE_AREA(ITER)
#undef ITER
        _LEN,
};

export struct Page {
    String name = ""s;
    usize number;
    bool blank;
};

export struct PageSpecifiedValues {
    using Areas = Array<Dom::PseudoElement, toUnderlyingType(PageArea::_LEN)>;

    Rc<SpecifiedValues> style;
    Areas _areas;

    PageSpecifiedValues(SpecifiedValues const& initial)
        : style(makeRc<SpecifiedValues>(initial)),
          _areas(Areas::fill([&](...) {
              Dom::PseudoElement pseudoElement;
              pseudoElement._specifiedValues = makeRc<SpecifiedValues>(initial);
              return pseudoElement;
          })) {}

    Dom::PseudoElement& area(PageArea margin) {
        return _areas[toUnderlyingType(margin)];
    }
};

// https://drafts.csswg.org/css-page-3/#at-page-rule

#define FOREACH_PAGE_PSEUDO(ITER) \
    ITER(FIRST, "first")          \
    ITER(BLANK, "blank")          \
    ITER(LEFT, "left")            \
    ITER(RIGHT, "right")

export enum struct PagePseudo {
#define ITER(ID, ...) ID,
    FOREACH_PAGE_PSEUDO(ITER)
#undef ITER
};

export struct PageSelector {
    String name = ""s;
    Vec<PagePseudo> pseudos;

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

    static PageSelector parse(Cursor<Css::Sst>& c) {
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

    static Vec<PageSelector> parseList(Cursor<Css::Sst>& c) {
        Vec<PageSelector> res;

        eatWhitespace(c);
        while (not c.ended()) {
            res.pushBack(parse(c));
            eatWhitespace(c);
        }

        return res;
    }

    bool match(Page const& page) const {
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

    void repr(Io::Emit& e) const {
        e("({} pseudos: {})", name, pseudos);
    }
};

export struct PageAreaRule {
    PageArea area;
    Vec<StyleProp> props;

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

    static Opt<PageAreaRule> parse(Css::Sst const& sst) {
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

    void apply(SpecifiedValues& c) const {
        for (auto const& prop : props) {
            prop.apply(c, c);
        }
    }

    void repr(Io::Emit& e) const {
        e("(page-margin-rule\nmargin: {}\nprops: {})", area, props);
    }
};

} // namespace Vaev::Style
