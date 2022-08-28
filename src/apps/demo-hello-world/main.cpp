#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/flow.h>
#include <karm-ui/text.h>

CliResult entryPoint(CliArgs args) {
    Ui::App app(Ui::center(Ui::text("Hello, world!")));
    return app.run(args);
}
