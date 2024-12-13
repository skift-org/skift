#pragma once

#include "origin.h"
#include "styles.h"

namespace Vaev::Style {

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

enum struct PageArea {
#define ITER(ID, ...) ID,
    FOREACH_PAGE_AREA(ITER)
#undef ITER
        _LEN,
};

struct Page {
    String name = ""s;
    usize number;
    bool blank;
};

struct PageComputedStyle {
    using Areas = Array<Strong<Computed>, toUnderlyingType(PageArea::_LEN)>;

    Strong<Computed> style;
    Areas _areas;

    PageComputedStyle(Computed const &initial)
        : style(makeStrong<Computed>(initial)),
          _areas(Areas::fill([&](...) {
              return makeStrong<Computed>(initial);
          })) {}

    Strong<Computed> area(PageArea margin) const {
        return _areas[toUnderlyingType(margin)];
    }
};

// https://drafts.csswg.org/css-page-3/#at-page-rule

#define FOREACH_PAGE_PSEUDO(ITER) \
    ITER(FIRST, "first")          \
    ITER(BLANK, "blank")          \
    ITER(LEFT, "left")            \
    ITER(RIGHT, "right")

enum struct PagePseudo {
#define ITER(ID, ...) ID,
    FOREACH_PAGE_PSEUDO(ITER)
#undef ITER
};

struct PageSelector {
    String name = ""s;
    Vec<PagePseudo> pseudos;

    static PageSelector parse(Cursor<Css::Sst> &c);

    static Vec<PageSelector> parseList(Cursor<Css::Sst> &c);

    bool match(Page const &page) const;

    void repr(Io::Emit &e) const;
};

struct PageAreaRule {
    PageArea area;
    Vec<StyleProp> props;

    static Opt<PageAreaRule> parse(Css::Sst const &sst);

    void apply(Style::Computed &c) const;

    void repr(Io::Emit &e) const;
};

struct PageRule {
    Vec<PageSelector> selectors;
    Vec<StyleProp> props;
    Vec<PageAreaRule> areas;

    static PageRule parse(Css::Sst const &sst);

    bool match(Page const &page) const;

    void apply(PageComputedStyle &c) const;

    void repr(Io::Emit &e) const;
};

} // namespace Vaev::Style
