#include "scafold.h"

#include "button.h"
#include "text.h"

namespace Karm::Ui {

void nop() {}

Ui::Child titlebar(Media::Icons icon, String title) {
    return Ui::spacing(8,
                       Ui::hflow(
                           4,
                           Ui::button(nop, Button::SUBTLE_ICON, icon, title),
                           Ui::spacer(),
                           Ui::button(nop, Button::SUBTLE_ICON, Media::Icons::MINUS),
                           Ui::button(nop, Button::SUBTLE_ICON, Media::Icons::PLUS),
                           Ui::button(nop, Button::DESTRUCTIVE_ICON, Media::Icons::WINDOW_CLOSE)));
}

struct Separator : public View<Separator> {
    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {1};
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.fillStyle(Gfx::ZINC700);
        g.fill(bound());
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
