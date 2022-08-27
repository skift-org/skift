#include "scafold.h"

#include "button.h"
#include "text.h"

namespace Karm::Ui {

static constexpr Ui::ButtonStyle SUBTLE = {
    .idleStyle = {
        .foregroundColor = Gfx::ZINC400,
    },
    .hoverStyle = {
        .borderRadius = Ui::Button::RADIUS,
        .borderWidth = 1,
        .borderColor = Gfx::ZINC500,
        .backgroundColor = Gfx::ZINC600,
    },
    .pressStyle = {
        .borderRadius = Ui::Button::RADIUS,
        .borderWidth = 1,
        .borderColor = Gfx::ZINC600,
        .backgroundColor = Gfx::ZINC700,
    },
};

static constexpr Ui::ButtonStyle CLOSE = {
    .idleStyle = {
        .borderRadius = Ui::Button::RADIUS,
        .foregroundColor = Gfx::ZINC400,
    },
    .hoverStyle = {
        .borderRadius = Ui::Button::RADIUS,
        .borderWidth = 1,
        .borderColor = Gfx::RED500,
        .backgroundColor = Gfx::RED600,
    },
    .pressStyle = {
        .borderRadius = Ui::Button::RADIUS,
        .borderWidth = 1,
        .borderColor = Gfx::RED600,
        .backgroundColor = Gfx::RED700,
    },
};

void nop() {}

Ui::Child titlebar(Media::Icons icon, String title) {
    return Ui::spacing(8,
                       Ui::hflow(
                           4,
                           Ui::button(nop, SUBTLE, icon, title),
                           Ui::spacer(),
                           Ui::button(nop, SUBTLE, Media::Icons::MINUS),
                           Ui::button(nop, SUBTLE, Media::Icons::PLUS),
                           Ui::button(nop, CLOSE, Media::Icons::WINDOW_CLOSE)));
}

} // namespace Karm::Ui
