module;

#include <karm-gfx/canvas.h>
#include <karm-math/align.h>

export module Karm.Kira:separator;

import Karm.Ui;

namespace Karm::Kira {

struct Separator : Ui::View<Separator> {
    Gfx::Color _color;

    Separator(Gfx::Color color)
        : _color(color) {}

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.fillStyle(_color);
        g.fill(bound());
        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {1};
    }
};

export Ui::Child separator(Gfx::Color color = Ui::GRAY800) {
    return makeRc<Separator>(color);
}

export Ui::Child separator(String text) {
    return Ui::hflow(
               4,
               Math::Align::VCENTER | Math::Align::HFILL | Math::Align::TOP_START,
               separator() | Ui::grow(),
               Ui::text(Ui::TextStyles::labelSmall().withColor(Ui::GRAY500), text) | Ui::insets({0, 6}),
               separator() | Ui::grow()
           ) |
           Ui::insets({0, 6});
}

} // namespace Karm::Kira