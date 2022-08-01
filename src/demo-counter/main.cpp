#include <karm-main/main.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>
#include <karm-ui/host.h>
#include <karm-ui/state.h>
#include <karm-ui/text.h>

constexpr auto inc(auto state) {
    return state + 1;
};

ExitCode entryPoint(CliArgs const &) {
    return Ui::show(0, [](auto state) {
        auto lbl = Ui::text("You clicked {} times!", state.value());
        auto btn = Ui::button(state.bind(inc), "Click me!");

        return Ui::flow(lbl, btn);
    });
}
