#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>

void nop() {}

Ui::Child buttons() {
    return Ui::vflow(
        32,
        Ui::primaryButton(nop, "PRIMARY BUTTON"),
        Ui::button(nop, "BUTTON"),
        Ui::outlineButton(nop, "OUTLINE BUTTON"),
        Ui::subtleButton(nop, "SUBTLE BUTTON"));
}

CliResult entryPoint(CliArgs args) {
    Ui::App app([]() {
        return Ui::center(
            Ui::spacing(
                32,
                buttons()));
    });

    return app.run(args);
}
