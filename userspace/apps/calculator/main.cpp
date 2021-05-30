#include <libio/Format.h>
#include <libwidget/Application.h>

#include "calculator/KeyPad.h"
#include "calculator/Screen.h"

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Widget::Application app;
    Calculator::Engine calculator{};

    // clang-format off

    auto win = Widget::window(
        Widget::vflow(
            0,
            {
                Widget::titlebar(Graphic::Icon::get("calculator-variant"), "Calculator"),
                screen(calculator),
                keypad(calculator)
            }
        )
    );

    // clang-format on

    calculator.did_update();

    win->resize_to_content();
    win->show();

    return app.run();
}
