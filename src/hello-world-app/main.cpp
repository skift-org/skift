#include <karm-main/main.h>
#include <karm-ui/context.h>

CliResult entryPoint(CliArgs) {
    return Ui::render(
        Ui::View(
            Ui::View(
                Ui::View()),
            Ui::View(
                Ui::View(
                    Ui::View())),
            Ui::View(
                Ui::View())));
}
