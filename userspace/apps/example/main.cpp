#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

using namespace Widget;

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Application app;

    // clang-format off

    auto win = window(
        vflow({
            titlebar(Graphic::Icon::get("duck"), "Example"),
            spacing(
                16,
                stateful<int>([](auto state, auto update) {
                    return hflow({
                        fill(label(IO::format("You clicked {} times", state))),
                        basic_button("Click Me!", [=] { update(state + 1); }),
                    });
                })
            ),
        })
    );

    // clang-format on

    win->show();

    return app.run();
}
