#include "scafold.h"

#include "button.h"
#include "dialog.h"
#include "text.h"

namespace Karm::Ui {

Ui::Child titlebar(Media::Icons icon, String title, TitlebarStyle style) {
    return Ui::spacing(
        8,
        Ui::hflow(
            4,
            Ui::button(
                [=](auto &n) {
                    Ui::showDialog(n, aboutDialog(icon, title));
                },
                Button::SUBTLE_ICON, icon, title),
            Ui::grow(),
            style == TitlebarStyle::DIALOG ? empty() : Ui::button(NOP, Button::SUBTLE_ICON, Media::Icons::MINUS),
            style == TitlebarStyle::DIALOG ? empty() : Ui::button(NOP, Button::SUBTLE_ICON, Media::Icons::CROP_SQUARE),
            Ui::button(NOP, Button::DESTRUCTIVE_ICON, Media::Icons::CLOSE)));
}

auto lookup(auto k, auto &m) {
    for (auto &kv : m) {
        if (kv.car == k) {
            return kv.cdr;
        }
    }

    panic("Key not found");
}

Child badge(BadgeStyle style, String text) {
    Array<Cons<BadgeStyle, Gfx::Color>, 4> styleToColor{
        {BadgeStyle::INFO, Gfx::BLUE400},
        {BadgeStyle::SUCCESS, Gfx::LIME400},
        {BadgeStyle::WARNING, Gfx::YELLOW400},
        {BadgeStyle::ERROR, Gfx::RED400},
    };

    Gfx::Color color = lookup(style, styleToColor);

    return Ui::box({
                       .borderRadius = 99,
                       .backgroundColor = color.withOpacity(0.1),
                       .foregroundColor = color,
                   },
                   Ui::spacing({8, 4}, Ui::text(text)));
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

Ui::Child separator() {
    return makeStrong<Separator>();
}

static constexpr BoxStyle TOOLBAR = {
    .backgroundColor = Gfx::ZINC800,
};

Ui::Child toolbar(Children children) {
    return box(TOOLBAR, Ui::spacing(8, hflow(4, children)));
}

} // namespace Karm::Ui
