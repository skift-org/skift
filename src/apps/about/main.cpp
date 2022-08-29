#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/scafold.h>

void nop() {}

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(Media::Icons::INFORMATION, "About");
    auto licenseButton = Ui::button(nop, Ui::Button::SUBTLE, "LICENSE");
    auto okButton = Ui::button(nop, Ui::Button::PRIMARY, "OK");

    auto content =
        Ui::vflow(
            8,
            Ui::spacer(),
            Ui::center(Ui::spacing(8, Ui::icon(Media::Icons::SNOWFLAKE, 72))),
            Ui::center(Ui::text(24, "skiftOS")),
            Ui::center(Ui::text("v0.0.1")),
            Ui::spacer(),
            Ui::center(Ui::text("Copyright © 2018-2023")),
            Ui::center(Ui::text("SMNX & contributors.")),
            Ui::empty(8),
            Ui::hflow(8, licenseButton, Ui::spacer(), okButton));

    auto layout =
        Ui::minSize(320,
                    Ui::vflow(
                        titlebar,
                        Ui::grow(
                            Ui::spacing(8, content))));

    return Ui::runApp(args, layout);
}
