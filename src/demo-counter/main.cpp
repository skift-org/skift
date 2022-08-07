#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>

int inc(int state) {
    return state + 1;
};

CliResult entryPoint(CliArgs args) {
    Ui::App app(0, [](auto state) {
        auto lbl = Ui::text("You clicked {} times!", state.value());
        auto btn = Ui::button(state.bind(inc), "Click me!");
        return Ui::flow(lbl, btn);
    });

    return app.run(args);
}
