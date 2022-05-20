#include <karm-main/main.h>
#include <karm-ui/button.h>
#include <karm-ui/label.h>
#include <karm-ui/state.h>
#include <karm-ui/window.h>

using namespace Karm;

ExitCode entryPoint(CliArgs const &) {
    Ui::Ctx app{[] {
        Ui::window([&] {
            auto state = Ui::useState(0);

            Ui::label("You, clicked {} times", *state);

            Ui::button("Click me!", [&](Ui::Event const &) {
                state.update([](auto &s) {
                    s++;
                });
            });
        });
    }};

    return app.run();
}
