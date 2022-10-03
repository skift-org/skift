#include "scafold.h"

#include "button.h"
#include "dialog.h"
#include "drag.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

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
                cond(style == TitlebarStyle::DEFAULT, button(NONE, ButtonStyle::subtle(), Media::Icons::MINUS)),
                cond(style == TitlebarStyle::DEFAULT, button(NONE, ButtonStyle::subtle(), Media::Icons::CROP_SQUARE)),
                button(
                    [](Node &n) {
                        Events::ExitEvent e{OK};
                        n.bubble(e);
                    },
                    ButtonStyle::subtle(), Media::Icons::CLOSE))));
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
                   .backgroundColor = color.withOpacity(0.1),
                   .foregroundColor = color,
               },
               spacing({8, 4}, text(t)));
}

struct Separator : public View<Separator> {
    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {1};
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.fillStyle(Gfx::ZINC700);
        g.fill(bound());
        g.restore();
    }
};

Child separator() {
    return makeStrong<Separator>();
}

static constexpr BoxStyle TOOLBAR = {
    .backgroundColor = Gfx::ZINC800,
};

Child toolbar(Children children) {
    return box(TOOLBAR, spacing(8, hflow(4, children)));
}

} // namespace Karm::Ui
