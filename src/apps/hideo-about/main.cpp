#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

namespace Hideo::About {

Ui::Child app() {
    return Ui::scafold({
        .icon = Mdi::INFORMATION,
        .title = "About",
        .titlebar = Ui::TitlebarStyle::DIALOG,
        .body = [] {
            auto titleText = Ui::headlineMedium(
                "skiftOS");

            auto bodyText = Ui::bodySmall(
                "Copyright © 2018-2024\n"
                "The skiftOS Developers\n"
                "All rights reserved.");

            auto transflagText = Ui::hflow(
                6,
                Ui::image(Media::loadImage("bundle://hideo-about/pride.qoi"_url).unwrap(), 4),
                Ui::bodySmall("Trans Rights are Human Rights"));

            auto licenseBtn = Ui::button(
                [](auto &n) {
                    Ui::showDialog(n, Ui::licenseDialog());
                },
                Ui::ButtonStyle::outline(),
                Mdi::LICENSE,
                "License");

            return Ui::vflow(
                       8,
                       Ui::hflow(
                           8,
                           titleText,
                           Ui::versionBadge() | Ui::center()),
                       Ui::empty(),
                       bodyText,
                       Ui::grow(NONE),
                       Ui::hflow(
                           8,
                           transflagText | Ui::vcenter() | Ui::grow(),
                           licenseBtn)) |
                   Ui::spacing(16);
        },
        .size = {460, 320},
    });
}

} // namespace Hideo::About

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::About::app());
}
