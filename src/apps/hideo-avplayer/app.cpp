module;

#include <karm-gfx/filters.h>
#include <karm-math/align.h>

export module Hideo.Avplayer;

import Mdi;
import Karm.Ui;
import Karm.Kira;
import Karm.Image;
import Karm.Ref;

using namespace Karm;

namespace Hideo::Avplayer {

Ui::Child player() {
    auto mediaContent =
        Ui::image("bundle://hideo-avplayer/images/bunny.qoi"_url) |
        Ui::cover() |
        Ui::vhclip();

    auto mediaControls =
        Ui::hflow(
            6,
            Math::Align::VCENTER | Math::Align::HFILL | Math::Align::TOP_START,
            Ui::hflow(
                Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::SKIP_PREVIOUS),
                Kr::separator(),
                Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::PLAY),
                Kr::separator(),
                Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::SKIP_NEXT)
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
            Ui::button(Ui::SINK<>, Ui::ButtonStyle::regular(), Mdi::VOLUME_HIGH),
            Ui::button(Ui::SINK<>, Ui::ButtonStyle::regular(), Mdi::FULLSCREEN),
            Ui::button(Ui::SINK<>, Ui::ButtonStyle::regular(), Mdi::COG)
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

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::PLAY_CIRCLE,
        .title = "Media Player"s,
        .body = [] {
            return player();
        },
    });
}

} // namespace Hideo::Avplayer
