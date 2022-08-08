#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>

int inc(int state) {
    return state + 1;
};

CliResult entryPoint(CliArgs args) {
    Ui::App app(Ui::center(Ui::text("Hello, world!")));

    return app.run(args);
}
