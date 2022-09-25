#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/view.h>

CliResult entryPoint(CliArgs args) {
    return Ui::runApp(args, Ui::text("Hello, world!"));
}
