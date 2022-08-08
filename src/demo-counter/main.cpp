#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>

int inc(int state) {
    return state + 1;
};

CliResult entryPoint(CliArgs args) {
    Ui::App app(0, [](auto state) {
        auto lbl = Ui::spacing(8, Ui::text("You clicked {} times!", state.value()));
        auto btn = Ui::button(state.bind(inc), "Click me!");

        return Ui::align(
            Layout::Align::VCENTER | Layout::Align::HFILL,
            Ui::flow(
                Layout::Flow::TOP_TO_BOTTOM,
                lbl,
                Ui::align(
                    Layout::Align::HCENTER | Layout::Align::TOP,
                    btn)));
    });

    return app.run(args);
}
