#include "scafold.h"

#include "dialog.h"
#include "drag.h"
#include "input.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

Child controls(TitlebarStyle style) {
    return hflow(8,
                 button(NONE, ButtonStyle::subtle(), Mdi::MINUS) | cond(style == TitlebarStyle::DEFAULT),
                 button(NONE, ButtonStyle::subtle(), Mdi::CROP_SQUARE) | cond(style == TitlebarStyle::DEFAULT),
                 button(
                     [](Node &n) {
                         Events::ExitEvent e{Ok()};
                         n.bubble(e);
                     },
                     ButtonStyle::subtle(), Mdi::CLOSE));
}

Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style) {
    return hflow(
               4,
               button(
                   [=](auto &n) {
                       showAboutDialog(n, icon, title);
                   },
                   ButtonStyle::subtle(), icon, title),
               grow(NONE),
               controls(style)) |
           spacing(8) |
           dragRegion();
}

auto lookup(auto k, auto &m) {
    for (auto &kv : m) {
        if (kv.car == k) {
            return kv.cdr;
        }
    }

    panic("Key not found");
}

Child badge(BadgeStyle style, String t) {
    using StyleToColor = Cons<BadgeStyle, Gfx::Color>;

    Array styleToColor = {
        StyleToColor{BadgeStyle::INFO, Gfx::BLUE400},
        StyleToColor{BadgeStyle::SUCCESS, Gfx::LIME400},
        StyleToColor{BadgeStyle::WARNING, Gfx::YELLOW400},
        StyleToColor{BadgeStyle::ERROR, Gfx::RED400},
    };

    Gfx::Color color = lookup(style, styleToColor);

    return text(t) |
           box({
               .padding = {8, 4},
               .borderRadius = 99,
               .backgroundPaint = color.withOpacity(0.1),
               .foregroundPaint = color,
           });
}

static BoxStyle TOOLBAR = {
    .backgroundPaint = Gfx::ZINC800,
};

Child toolbar(Children children) {
    return vflow(
        hflow(0, children) |
            spacing(8) |
            box(TOOLBAR),
        separator());
}

Ui::Child bottombar(Children children) {
    return vflow(
        separator(),
        hflow(4, children) |
            spacing(8) |
            box(TOOLBAR));
}

} // namespace Karm::Ui
