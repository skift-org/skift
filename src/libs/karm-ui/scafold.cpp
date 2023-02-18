#include "scafold.h"

#include "dialog.h"
#include "drag.h"
#include "input.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

Child controls(TitlebarStyle style) {
    return hflow(8,
                 cond(style == TitlebarStyle::DEFAULT, button(NONE, ButtonStyle::subtle(), Media::Icons::MINUS)),
                 cond(style == TitlebarStyle::DEFAULT, button(NONE, ButtonStyle::subtle(), Media::Icons::CROP_SQUARE)),
                 button(
                     [](Node &n) {
                         Events::ExitEvent e{Ok()};
                         n.bubble(e);
                     },
                     ButtonStyle::subtle(), Media::Icons::CLOSE));
}

Child titlebar(Media::Icons icon, String title, TitlebarStyle style) {
    return dragRegion(
        spacing(
            8,
            hflow(
                4,
                button(
                    [=](auto &n) {
                        showAboutDialog(n, icon, title);
                    },
                    ButtonStyle::subtle(), icon, title),
                grow(),
                controls(style))));
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

    return box({
                   .borderRadius = 99,
                   .backgroundPaint = color.withOpacity(0.1),
                   .foregroundPaint = color,
               },
               spacing({8, 4}, text(t)));
}

static BoxStyle TOOLBAR = {
    .backgroundPaint = Gfx::ZINC800,
};

Child toolbar(Children children) {
    return vflow(box(TOOLBAR, spacing(8, hflow(4, children))), separator());
}

} // namespace Karm::Ui
