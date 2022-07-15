#include <karm-main/main.h>
#include <karm-ui/context.h>

ExitCode entryPoint(CliArgs const &) {
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
