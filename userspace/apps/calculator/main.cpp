#include <libio/Format.h>

#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Label.h>
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
                button(Button::OUTLINE, Graphic::Icon::get("percent")),

                button(Button::OUTLINE, "CE", [&] {
                    calculator.clear_all();
                }),

                button(Button::OUTLINE, "C", [&] {
                    calculator.clear();
                }),

                button(Button::OUTLINE, Graphic::Icon::get("backspace-outline")),

                button(Button::OUTLINE, "1/x", [&] {
                    calculator.enter_operation(Operation::RECIPROCAL);
                }),

                button(Button::OUTLINE, Graphic::Icon::get("exponent"), [&] {
                    calculator.enter_operation(Operation::POWER);
                }),

                button(Button::OUTLINE, Graphic::Icon::get("square-root"), [&] {
                    calculator.enter_operation(Operation::SQRT);
                }),

                button(Button::OUTLINE, Graphic::Icon::get("slash-forward"), [&] {
                    calculator.enter_operation(Operation::DIVIDE);
                }),

                button(Button::OUTLINE, "7", [&] {
                    calculator.enter_number(7);
                }),

                button(Button::OUTLINE, "8", [&] {
                    calculator.enter_number(8);
                }),

                button(Button::OUTLINE, "9", [&] {
                    calculator.enter_number(9);
                }),

                button(Button::OUTLINE, Graphic::Icon::get("close"), [&] {
                    calculator.enter_operation(Operation::MULTIPLY);
                }),

                button(Button::OUTLINE, "4", [&] {
                    calculator.enter_number(4);
                }),

                button(Button::OUTLINE, "5", [&] {
                    calculator.enter_number(5);
                }),

                button(Button::OUTLINE, "6", [&] {
                    calculator.enter_number(6);
                }),

                button(Button::OUTLINE, Graphic::Icon::get("minus"), [&] {
                    calculator.enter_operation(Operation::SUBSTRACT);
                }),

                button(Button::OUTLINE, "1", [&] {
                    calculator.enter_number(1);
                }),

                button(Button::OUTLINE, "2", [&] {
                    calculator.enter_number(2);
                }),

                button(Button::OUTLINE, "3", [&] {
                    calculator.enter_number(3);
                }),

                button(Button::OUTLINE, Graphic::Icon::get("plus"), [&] {
                    calculator.enter_operation(Operation::ADD);
                }),

                button(Button::OUTLINE, Graphic::Icon::get("plus-minus-variant")),

                button(Button::OUTLINE, "0", [&] {
                    calculator.enter_number(0);
                }),

                button(Button::OUTLINE, "."),

                button(Button::FILLED, Graphic::Icon::get("equal"), [&] {
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
