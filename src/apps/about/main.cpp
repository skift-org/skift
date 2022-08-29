#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/scafold.h>

void nop() {}

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(Media::Icons::INFORMATION, "About");

    auto content =
        Ui::minSize(320,
                    Ui::vflow(
                        8,
                        Ui::spacer(),
                        Ui::center(Ui::spacing(8, Ui::icon(Media::Icons::SNOWFLAKE, 56))),
                        Ui::center(Ui::text(24, "skiftOS")),
                        Ui::center(Ui::text("v0.0.1")),
                        Ui::spacer(),
                        Ui::center(Ui::text("Copyright © 2018-2023")),
                        Ui::center(Ui::text("SMNX & contributors.")),
                        Ui::spacer(),
                        Ui::spacing({8, 0, 8, 8},
                                    Ui::button(nop, "ok"))));

    auto layout =
        Ui::vflow(
            titlebar,
            content);

    return Ui::runApp(args, layout);
}
