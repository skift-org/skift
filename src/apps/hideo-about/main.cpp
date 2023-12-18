#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

namespace Hideo::About {

Ui::Child app() {
    auto licenseBtn = Ui::button(
        [](auto &n) {
            Ui::showDialog(n, Ui::licenseDialog());
        },
        Ui::ButtonStyle::subtle(),
        Mdi::LICENSE,
        "LICENSE");

    auto closeBtn = Ui::button(
        Ui::bindBubble<Events::ExitEvent>(),
        Ui::ButtonStyle::primary(),
        "CLOSE");

    auto content = Ui::spacing(
        16,
        Ui::vflow(
            8,
            Ui::hflow(
                8,
                Ui::headlineMedium("skiftOS"),
                Ui::versionBadge() | Ui::center()),
            Ui::empty(),
            Ui::text2("Copyright © 2018-2023\nThe skiftOS Developers\nAll rights reserved."),
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
        .size = {460, 320},
    });
}

} // namespace Hideo::About

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::About::app());
}
