#include "scafold.h"

#include "dialog.h"
#include "drag.h"
#include "input.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

static BoxStyle TOOLBAR = {
    .backgroundPaint = GRAY900,
};

Child aboutButton(Mdi::Icon icon, String title) {
    return button(
        rbind(showAboutDialog, icon, title),
        ButtonStyle::subtle(), icon, title);
}

Child controls(TitlebarStyle style) {
    return hflow(12,
                 button(bindBubble<Events::RequestMinimizeEvent>(), ButtonStyle::subtle(), Mdi::MINUS) |
                     cond(style == TitlebarStyle::DEFAULT),
                 button(bindBubble<Events::RequestMaximizeEvent>(), ButtonStyle::subtle(), Mdi::CROP_SQUARE) |
                     cond(style == TitlebarStyle::DEFAULT),
                 button(bindBubble<Events::ExitEvent>(Ok()), ButtonStyle::subtle(), Mdi::CLOSE));
}

Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style) {
    return hflow(
               4,
               aboutButton(icon, title),
               grow(NONE),
               controls(style)) |
           spacing(8) |
           dragRegion() | box(TOOLBAR);
}

Child titlebar(Mdi::Icon icon, String title, Child tabs, TitlebarStyle style) {
    return hflow(
               4,
               aboutButton(icon, title),
               tabs | Ui::grow(),
               controls(style)) |
           spacing(8) |
           dragRegion() | box(TOOLBAR);
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
