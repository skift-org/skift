#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/scafold.h>

void nop(Ui::Node &) {}

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(
        Media::Icons::INFORMATION,
        "About",
        Ui::TitlebarStyle::DIALOG);

    auto content = Ui::vflow(
        8,
        Layout::Align::CENTER,

        Ui::icon(Media::Icons::SNOWFLAKE, 72),
        Ui::text(24, "skiftOS"),
        Ui::empty(),
        Ui::badge(Ui::BadgeStyle::INFO, "v0.1.0"),
        Ui::empty(),
        Ui::text("Copyright © 2018-2023"),
        Ui::text("SMNX & contributors."),
        Ui::empty());

    auto controls = Ui::hflow(
        Ui::button(nop, Ui::Button::SUBTLE, "LICENSE"),
        Ui::grow(),
        Ui::button(nop, Ui::Button::PRIMARY, "OK"));

    auto layout = Ui::dialogLayer(Ui::minSize(
        320,
        Ui::vflow(
            titlebar,
            Ui::grow(content),
            Ui::spacing(8, controls))));

    return Ui::runApp(args, layout);
}
