#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

using namespace Widget;

namespace Example
{

struct Application : public Widget::Application
{
    OwnPtr<Window> build() override
    {
        // clang-format off

        return window(
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
    }
};

} // namespace Example

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Example::Application app;
    return app.run();
}
