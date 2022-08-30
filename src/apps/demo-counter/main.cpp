#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>
#include <karm-ui/scafold.h>

int inc(int state) {
    return state + 1;
};

CliResult entryPoint(CliArgs args) {
    auto content = Ui::state(0, [](auto state) {
        auto lbl = Ui::text("You clicked {} times!", state.value());
        auto btn = Ui::button(state.bind(inc), "CLICK ME!");

        return Ui::spacing(
            8,
            Ui::hflow(
                Ui::minSize({256, Ui::Sizing::UNCONSTRAINED}, Ui::vcenter(lbl)),
                btn));
    });

    auto layout =
        Ui::vflow(
            Ui::titlebar(
                Media::Icons::COUNTER,
                "Counter",
                Ui::TitlebarStyle::DIALOG),
            Ui::grow(content));

    return Ui::runApp(args, layout);
}
