#include <hideo-base/dialogs.h>
#include <hideo-base/scafold.h>
#include <karm-base/witty.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>

namespace Hideo::About {

Ui::Child app() {
    return scafold({
        .icon = Mdi::INFORMATION,
        .title = "About"s,
        .titlebar = TitlebarStyle::DIALOG,
        .body = [] {
            auto titleText = Ui::headlineMedium(
                "skiftOS"
            );

            auto bodyText = Ui::bodySmall(
                "Copyright © 2018-2024\n"
                "The skiftOS Developers\n"
                "All rights reserved."
            );

            auto inspireMe = Ui::state(Sys::now().val(), [](auto v, auto bind) {
                auto body = Ui::hflow(
                    8,
                    Layout::Align::CENTER,
                    Ui::image(Media::loadImage("bundle://hideo-about/pride.qoi"_url).unwrap(), 4),
                    Ui::bodySmall(wholesome(v))
                );

                return body |
                       Ui::spacing({12, 6, 16, 6}) |
                       Ui::minSize({Ui::UNCONSTRAINED, 36}) |
                       Ui::button(bind(v + 1), Ui::ButtonStyle::subtle());
            });

            auto licenseBtn = Ui::button(
                [](auto &n) {
                    Ui::showDialog(n, licenseDialog());
                },
                Ui::ButtonStyle::outline(),
                Mdi::LICENSE,
                "License"
            );

            return Ui::vflow(
                       8,
                       Ui::hflow(
                           8,
                           titleText,
                           versionBadge() | Ui::center()
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
                   Ui::spacing(16);
        },
        .size = {460, 320},
    });
}

} // namespace Hideo::About

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::About::app());
}
