#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

using namespace Widget;

int main(int argc, char const *argv[])
{
    // clang-format off

    UNUSED(argc);
    UNUSED(argv);

    Application app;

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

    win->show();

    return app.run();

    // clang-format on
}
