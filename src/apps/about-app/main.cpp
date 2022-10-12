#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/view.h>

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(
        Media::Icons::INFORMATION,
        "About",
        Ui::TitlebarStyle::DIALOG);

    auto content = Ui::vflow(
        8,
        Layout::Align::CENTER,

        Ui::icon(Media::Icons::SNOWFLAKE, 72),
        Ui::text(Ui::TextStyle::title1(), "skiftOS"),
        Ui::empty(),
        Ui::badge(Ui::BadgeStyle::INFO, "v0.1.0"),
        Ui::empty(),
        Ui::text("Copyright © 2018-2022"),
        Ui::text("SMNX & contributors."),
        Ui::empty());

    auto licenseBtn = Ui::button(
        NONE,
        Ui::ButtonStyle::subtle().withRadius(999),
        Media::Icons::LICENSE,
        "LICENSE");

    auto wrapper = Ui::spacing(
        32,
        Ui::vflow(
            32,
            Ui::grow(Ui::center(content)),
            Ui::center(licenseBtn)));

    auto layout = Ui::dialogLayer(
        Ui::minSize(
            420,
            Ui::vflow(
                titlebar,
                Ui::grow(Ui::center(wrapper)))));

    return Ui::runApp(args, layout);
}
