#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

namespace About {

Ui::Child app() {
    auto logo = Ui::icon(Mdi::SNOWFLAKE, 96) |
                Ui::center() |
                Ui::bound() |
                Ui::box({
                    .padding = 64,
                    .backgroundPaint = Ui::GRAY900,
                    .foregroundPaint = Ui::GRAY50,
                });

    auto licenseBtn = Ui::button(
        NONE,
        Ui::ButtonStyle::subtle(),
        Mdi::LICENSE,
        "LICENSE");

    auto closeBtn = Ui::button(
        [](Ui::Node &n) {
            Events::ExitEvent e{Ok()};
            n.bubble(e);
        },
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

    auto titlebar = Ui::titlebar(
        Mdi::INFORMATION,
        "About",
        Ui::TitlebarStyle::DIALOG);

    return Ui::vflow(titlebar, Ui::separator(), Ui::hflow(logo, Ui::separator(), content | Ui::grow()) | Ui::grow()) | Ui::pinSize({560, 360}) |
           Ui::dialogLayer();
}

} // namespace About

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, About::app());
}
