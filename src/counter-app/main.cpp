#include <karm-main/main.h>
#include <karm-ui/button.h>
#include <karm-ui/label.h>
#include <karm-ui/state.h>
#include <karm-ui/window.h>

using namespace Karm::Ui;

ExitCode entryPoint(CliArgs const &) {
    Ui app{[] {
        window([&] {
            auto state = useState(0);

            label("You, clicked {} times", *state);

            button("Click me!", [&](Event const &) {
                state.update([](auto &s) {
                    s++;
                });
            });
        });
    }};

    return app.run();
}
