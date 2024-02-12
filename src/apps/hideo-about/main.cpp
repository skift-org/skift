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

            auto licenseBtn = Ui::button(
                [](auto &n) {
                    Ui::showDialog(n, Ui::licenseDialog());
                },
                Ui::ButtonStyle::subtle(),
                Mdi::LICENSE,
                "License");

            auto closeBtn = Ui::button(
                Ui::bindBubble<Events::RequestExitEvent>(),
                Ui::ButtonStyle::primary(),
                "Close");

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
                           licenseBtn,
                           Ui::grow(NONE),
                           closeBtn)) |
                   Ui::spacing(16);
        },
        .size = {460, 320},
    });
}

} // namespace Hideo::About

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::About::app());
}
