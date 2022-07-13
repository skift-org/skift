#include <karm-main/main.h>
#include <karm-ui/context.h>

ExitCode entryPoint(CliArgs const &) {
    return Ui::render(
        Ui::Text(String{"Hello, app world!"}));
}
