#include <karm-main/main.h>
#include <karm-ui/context.h>

ExitCode entryPoint(CliArgs const &) {
    auto app =
        Ui::Window(
            Ui::Text(String{"Hello, app world!"}));

    return Cli::SUCCESS;
}
