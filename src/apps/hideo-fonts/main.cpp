#include <karm-gfx/prose.h>
#include <karm-math/align.h>
#include <karm-sys/entry.h>

import Karm.Kira;
import Karm.Ui;
import Karm.Font;
import Mdi;

using namespace Karm;

namespace Hideo::Fonts {

struct State {
    Font::Database fontBook;
    Opt<Symbol> fontFamily = NONE;
    Opt<Rc<Gfx::Fontface>> fontFace = NONE;

    State(Font::Database fontBook) : fontBook(fontBook) {}

    bool canGoBack() const {
        return fontFace || fontFamily;
    }
};

struct GoBack {};

struct SelectFamily {
    Symbol family = ""_sym;
};

struct SelectFace {
    Rc<Gfx::Fontface> id;
};

using Action = Union<GoBack, SelectFamily, SelectFace>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{
        [&](GoBack) {
            if (s.fontFace) {
                s.fontFace = NONE;
            } else if (s.fontFamily) {
                s.fontFamily = NONE;
            }
        },
        [&](SelectFamily a) {
            s.fontFamily = a.family;
        },
        [&](SelectFace a) {
            s.fontFace = a.id;
        },
    });

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

static constexpr Str PANGRAM = "All beings born free, equal in dignity, rights—justice demanded, voice exhorted, zealously championed worldwide";

// MARK: All Families ----------------------------------------------------------

Ui::Child allFamiliesItem(State const& s, Symbol family) {
    auto& fontBook = s.fontBook;
    auto nStyle = s.fontBook.queryFamily(family).len();
    auto fontface = fontBook.queryClosest(family).unwrap();

    Gfx::Font font{
        .fontface = fontface,
        .fontsize = 36,
    };

    return Ui::vflow(
               8,
               Ui::labelMedium(Ui::GRAY500, "{} · {} {}", family, nStyle, nStyle == 1 ? "Style" : "Styles"),
               Ui::text(Gfx::ProseStyle{font}, PANGRAM)
           ) |
           Ui::insets({8, 0, 8, 12}) |
           Ui::hclip() |
           Ui::button(Model::bind<SelectFamily>(family), Ui::ButtonStyle::outline());
}

Ui::Child allFamiliesContent(State const& s) {
    Ui::Children children;
    auto& fontBook = s.fontBook;
    auto families = fontBook.families();
    for (auto const& family : families) {
        children.pushBack(allFamiliesItem(s, family));
    }

    return Ui::vflow(8, children) |
           Ui::insets(16) |
           Ui::vscroll();
}

// MARK: Family ----------------------------------------------------------------

Ui::Child fontfaceTag(Str str) {
    return Kr::badge(Ui::GRAY400, Io::toParamCase(str).unwrap());
}

Ui::Child fontfaceTags(Gfx::FontAttrs const& attrs) {
    Ui::Children children;
    if (attrs.monospace == Gfx::Monospace::YES) {
        children.pushBack(fontfaceTag("monospace"s));
    }

    if (attrs.style != Gfx::FontStyle::NORMAL) {
        children.pushBack(fontfaceTag(Io::toStr(attrs.style)));
    }

    if (attrs.stretch != Gfx::FontStretch::NORMAL) {
        children.pushBack(fontfaceTag(Io::toStr(attrs.stretch)));
    }

    if (attrs.weight != Gfx::FontWeight::REGULAR) {
        children.pushBack(fontfaceTag(Io::toStr(attrs.weight)));
    }

    return Ui::hflow(4, children);
}

Ui::Child familyItem(State const&, Rc<Gfx::Fontface> fontface) {
    auto attrs = fontface->attrs();

    Gfx::Font font{
        .fontface = fontface,
        .fontsize = 36,
    };

    return Ui::vflow(
               8,
               Ui::labelMedium(Ui::GRAY500, "{}", attrs.family),
               Ui::text(Gfx::ProseStyle{font}, PANGRAM),
               fontfaceTags(attrs)
           ) |
           Ui::insets({8, 0, 8, 12}) |
           Ui::hclip() |
           Ui::button(Model::bind<SelectFace>(fontface), Ui::ButtonStyle::outline());
}

Ui::Child familyContent(State const& s) {
    Ui::Children children;
    auto& fontBook = s.fontBook;
    auto fontfaces = fontBook.queryFamily(s.fontFamily.unwrap());

    auto header = Ui::labelSmall("{}", s.fontFamily.unwrap()) | Ui::insets({6, 16});

    for (auto const& fontface : fontfaces) {
        children.pushBack(familyItem(s, fontface));
    }

    return Ui::vflow(
        header,
        Kr::separator(),
        Ui::vflow(8, children) | Ui::insets(16) | Ui::vscroll() | Ui::grow()
    );
}

// MARK: Fontface --------------------------------------------------------------

Ui::Child pangrams(Rc<Gfx::Fontface> fontface) {
    f64 size = 12;
    Ui::Children children;

    for (isize i = 0; i < 12; i++) {
        Gfx::Font font{
            .fontface = fontface,
            .fontsize = size,
        };
        children.pushBack(Ui::text(Gfx::ProseStyle{font}, PANGRAM));
        size *= 1.2;
    }

    return Ui::vflow(8, children) |
           Ui::insets(16) |
           Ui::vhscroll();
}

Ui::Child fontfaceContent(State const& s) {
    auto fontface = s.fontFace.unwrap();
    auto attrs = fontface->attrs();

    return Ui::vflow(
        Ui::hflow(
            0,
            Math::Align::CENTER,
            Ui::labelSmall(attrs.normal() ? "{}" : "{}  · ", attrs.family),
            fontfaceTags(attrs)
        ) | Ui::insets({6, 16}),
        Kr::separator(),
        pangrams(fontface)
    );
}

Ui::Child appContent(State const& s) {
    if (s.fontFace) {
        return fontfaceContent(s);
    } else if (s.fontFamily) {
        return familyContent(s);
    } else {
        return allFamiliesContent(s);
    }
}

Ui::Child app(Font::Database db) {
    return Ui::reducer<Model>(db, [](State const& s) {
        return Kr::scaffold({
            .icon = Mdi::FORMAT_FONT,
            .title = "Fonts"s,
            .startTools = [&] -> Ui::Children {
                return {Ui::button(
                    Model::bindIf<GoBack>(s.canGoBack()),
                    Ui::ButtonStyle::subtle(),
                    Mdi::ARROW_LEFT
                )};
            },
            .body = [&] {
                return appContent(s);
            },
        });
    });
}

} // namespace Hideo::Fonts

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    Font::Database db;
    co_try$(db.loadSystemFonts());
    co_return co_await Ui::runAsync(ctx, Hideo::Fonts::app(db));
}
