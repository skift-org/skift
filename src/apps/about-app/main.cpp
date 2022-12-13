#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/view.h>

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(
        Media::Icons::INFORMATION,
        "About",
        Ui::TitlebarStyle::DIALOG);

    auto logo = Ui::grow(Ui::box(
        {
            .backgroundPaint = Gfx::WHITE,
            .foregroundPaint = Gfx::BLACK,
        },
        Ui::bound(Ui::center(Ui::icon(Media::Icons::SNOWFLAKE, 128)))));

    auto licenseBtn = Ui::button(
        NONE,
        Ui::ButtonStyle::subtle().withRadius(999),
        Media::Icons::LICENSE,
        "LICENSE");

    auto closeBtn = Ui::button(
        [](Ui::Node &n) {
            Events::ExitEvent e{OK};
            n.bubble(e);
        },
        Ui::ButtonStyle::primary(),
        "OK");

    auto content = Ui::spacing(
        16,
        Ui::vflow(
            8,
            Ui::hflow(8,
                      Ui::text(Ui::TextStyle::titleLarge(), "skiftOS"),
                      Ui::center(Ui::badge(Ui::BadgeStyle::INFO, "v0.1.0"))),
            Ui::empty(),
            Ui::text("Copyright © 2018-2022"),
            Ui::text("SMNX & contributors."),
            Ui::grow(),
            Ui::hflow(8, licenseBtn, Ui::grow(), closeBtn)));

    auto wrapper =
        Ui::hflow(logo, Ui::grow(content));

    auto layout = Ui::dragRegion(
        Ui::minSize(
            {500, 300},
            wrapper));

    return Ui::runApp(args, layout);
}
