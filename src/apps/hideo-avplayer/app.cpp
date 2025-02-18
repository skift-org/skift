#include <karm-image/loader.h>
#include <karm-kira/scaffold.h>
#include <karm-kira/slider.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <mdi/cog.h>
#include <mdi/fullscreen.h>
#include <mdi/pause.h>
#include <mdi/play-circle.h>
#include <mdi/play.h>
#include <mdi/skip-next.h>
#include <mdi/skip-previous.h>
#include <mdi/volume-high.h>

#include "app.h"

namespace Hideo::Avplayer {

Ui::Child player() {
    auto mediaContent =
        Ui::image(
            Image::load("bundle://hideo-avplayer/images/bunny.qoi"_url).unwrap()
        ) |
        Ui::cover() |
        Ui::vhclip();

    auto mediaControls =
        Ui::hflow(
            6,
            Math::Align::VCENTER | Math::Align::HFILL | Math::Align::TOP_START,
            Ui::hflow(
                Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::SKIP_PREVIOUS),
                Ui::separator(),
                Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::PLAY),
                Ui::separator(),
                Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::SKIP_NEXT)
            ) | Ui::box({
                    .borderRadii = 4,
                    .borderFill = Ui::GRAY700,
                    .backgroundFill = Ui::GRAY800,
                }),
            Ui::empty(4),
            Ui::labelMedium("00:00"),
            Ui::empty(8) |
                Ui::box({
                    .margin = {0, 8},
                    .borderRadii = 99,
                    .backgroundFill = Ui::ACCENT500,
                }) |
                Ui::grow(),
            Ui::labelMedium("00:00"),
            Ui::empty(4),
            Ui::button(Ui::NOP, Ui::ButtonStyle::regular(), Mdi::VOLUME_HIGH),
            Ui::button(Ui::NOP, Ui::ButtonStyle::regular(), Mdi::FULLSCREEN),
            Ui::button(Ui::NOP, Ui::ButtonStyle::regular(), Mdi::COG)
        ) |
        Ui::insets(8) |
        Ui::box({
            .backgroundFill = Ui::GRAY900.withOpacity(0.6),
        }) |
        Ui::backgroundFilter(Gfx::BlurFilter{16});

    return Ui::stack(
               mediaContent,
               Ui::vflow(
                   Ui::grow(NONE),
                   mediaControls
               )
           ) |
           Ui::grow();
}

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::PLAY_CIRCLE,
        .title = "Media Player"s,
        .body = [] {
            return player();
        },
    });
}

} // namespace Hideo::Avplayer
