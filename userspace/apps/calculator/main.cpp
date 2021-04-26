#include <libio/Format.h>

#include <libwidget/Application.h>

#include <libwidget/Layouts.h>
#include <libwidget/TitleBar.h>

#include "calculator/Calculator.h"

int main(int, char **)
{
    using namespace Widget;

    Calculator calculator{};

    // clang-format off

    auto titlebar_container = titlebar(Graphic::Icon::get("calculator-variant"), "Calculator");

    auto screen_label = label("#value", Anchor::CENTER);
    auto calculator_observer = calculator.observe([&](Calculator &calculator) {
        screen_label->text(IO::format("{}", calculator.screen()));
    });

    auto screen_container = panel(
        fill(
            spacing(
                16,
                screen_label
            )
        )
    );

    auto buttons_container = spacing(
        6,
        grid(
            4, 6, 4, 4,
            {
                outlined_button(Graphic::Icon::get("percent")),

                outlined_button("CE", [&] {
                    calculator.clear_all();
                }),

                outlined_button("C", [&] {
                    calculator.clear();
                }),

                outlined_button(Graphic::Icon::get("backspace-outline")),

                outlined_button("1/x", [&] {
                    calculator.enter_operation(Operation::RECIPROCAL);
                }),

                outlined_button(Graphic::Icon::get("exponent"), [&] {
                    calculator.enter_operation(Operation::POWER);
                }),

                outlined_button(Graphic::Icon::get("square-root"), [&] {
                    calculator.enter_operation(Operation::SQRT);
                }),

                outlined_button(Graphic::Icon::get("slash-forward"), [&] {
                    calculator.enter_operation(Operation::DIVIDE);
                }),

                outlined_button("7", [&] {
                    calculator.enter_number(7);
                }),

                outlined_button("8", [&] {
                    calculator.enter_number(8);
                }),

                outlined_button("9", [&] {
                    calculator.enter_number(9);
                }),

                outlined_button(Graphic::Icon::get("close"), [&] {
                    calculator.enter_operation(Operation::MULTIPLY);
                }),

                outlined_button("4", [&] {
                    calculator.enter_number(4);
                }),

                outlined_button("5", [&] {
                    calculator.enter_number(5);
                }),

                outlined_button("6", [&] {
                    calculator.enter_number(6);
                }),

                outlined_button(Graphic::Icon::get("minus"), [&] {
                    calculator.enter_operation(Operation::SUBSTRACT);
                }),

                outlined_button("1", [&] {
                    calculator.enter_number(1);
                }),

                outlined_button("2", [&] {
                    calculator.enter_number(2);
                }),

                outlined_button("3", [&] {
                    calculator.enter_number(3);
                }),

                outlined_button(Graphic::Icon::get("plus"), [&] {
                    calculator.enter_operation(Operation::ADD);
                }),

                outlined_button(Graphic::Icon::get("plus-minus-variant")),

                outlined_button("0", [&] {
                    calculator.enter_number(0);
                }),

                outlined_button("."),

                filled_button(Graphic::Icon::get("equal"), [&] {
                    calculator.equal();
                }),
            }
        )
    );

    auto win = window(
        vflow(
            0,
            {
                titlebar_container,
                screen_container,
                buttons_container
            }
        )
    );
    // clang-format on

    calculator.did_update();

    win->resize_to_content();
    win->show();

    return Application::the()->run();
}
