#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

namespace About {

Ui::Child app() {
    auto licenseBtn = Ui::button(
        NONE,
        Ui::ButtonStyle::subtle(),
        Mdi::LICENSE,
        "LICENSE");

    auto closeBtn = Ui::button(
        Ui::bindBubble<Events::ExitEvent>(Ok()),
        Ui::ButtonStyle::primary(),
        "OK");

    auto content = Ui::spacing(
        16,
        Ui::vflow(
            8,
            Ui::hflow(8,
                      Ui::headlineMedium("skiftOS"),
                      Ui::badge(Ui::BadgeStyle::INFO, "v0.1.0") | Ui::center()),
            Ui::empty(),
            Ui::text("Copyright © 2018-2023"),
            Ui::text("SMNX Research & contributors."),
            Ui::text("All rights reserved."),
            Ui::grow(NONE),
            Ui::hflow(
                8,
                licenseBtn,
                Ui::grow(NONE),
                closeBtn)));

    return Ui::scafold({
        .icon = Mdi::INFORMATION,
        .title = "About",
        .titlebar = Ui::TitlebarStyle::DIALOG,
        .body = content,
        .size = {460, 360},
    });
}

} // namespace About

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, About::app());
}
