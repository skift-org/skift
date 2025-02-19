module;

#include <karm-base/witty.h>
#include <karm-image/loader.h>
#include <karm-kira/about-dialog.h>
#include <karm-kira/badge.h>
#include <karm-kira/scaffold.h>
#include <karm-sys/time.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>
#include <mdi/information.h>
#include <mdi/license.h>

export module Hideo.About;

namespace Hideo::About {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::INFORMATION,
        .title = "About"s,
        .titlebar = Kr::TitlebarStyle::DIALOG,
        .body =
            [] {
                auto titleText = Ui::headlineMedium("skiftOS");

                auto bodyText =
                    Ui::bodySmall(
                        "Copyright © 2018-2024 The skiftOS Developers\n"
                        "Copyright © 2024 Odoo S.A.\n"
                        "\n"
                        "All rights reserved."
                    );

                auto inspireMe = Ui::state(Sys::now().val(), [](auto v, auto bind) {
                    auto body = Ui::hflow(
                        8, Math::Align::CENTER,
                        Ui::image(Image::load("bundle://hideo-about/pride.qoi"_url).unwrap(), 4),
                        Ui::bodySmall(wholesome(v))
                    );

                    return body | Ui::insets({6, 16, 6, 12}) |
                           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
                           Ui::button(bind(v + 1), Ui::ButtonStyle::subtle());
                });

                auto licenseBtn = Ui::button(
                    [](auto& n) {
                        Ui::showDialog(n, Kr::licenseDialog());
                    },
                    Ui::ButtonStyle::outline(), Mdi::LICENSE, "License"
                );

                return Ui::vflow(
                           8,
                           Ui::hflow(
                               8, titleText,
                               Kr::versionBadge() | Ui::center()
                           ),
                           Ui::empty(),
                           bodyText,
                           Ui::grow(NONE),
                           Ui::hflow(
                               8,
                               inspireMe | Ui::vcenter() | Ui::grow(),
                               licenseBtn
                           )
                       ) |
                       Ui::insets(16);
            },
        .size = {460, 320},
    });
}

} // namespace Hideo::About
