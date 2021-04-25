#include <libio/Format.h>

#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Label.h>
#include <libwidget/Layouts.h>
#include <libwidget/Panel.h>
#include <libwidget/TitleBar.h>

#include "calculator/Calculator.h"

int main(int argc, char **argv)
{
    using namespace Widget;

    Application::initialize(argc, argv);

    Calculator calculator{};

    // clang-format off

    auto titlebar = Widget::titlebar(Graphic::Icon::get("calculator-variant"), "Calculator");

    auto screen_label = Widget::label("#value", Anchor::CENTER);
    auto calculator_observer = calculator.observe([&](Calculator &calculator) {
        screen_label->text(IO::format("{}", calculator.screen()));
    });

    auto screen_container = Widget::panel(
        Widget::fill(
            Widget::spacing(
                16,
                screen_label
            )
        )
    );

    auto buttons_container = Widget::spacing(
        6,
        Widget::grid(
            4, 6, 4, 4,
            {
                Widget::button(Button::OUTLINE, Graphic::Icon::get("percent")),

                Widget::button(Button::OUTLINE, "CE", [&] {
                    calculator.clear_all();
                }),

                Widget::button(Button::OUTLINE, "C", [&] {
                    calculator.clear();
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("backspace-outline")),

                Widget::button(Button::OUTLINE, "1/x", [&] {
                    calculator.enter_operation(Operation::RECIPROCAL);
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("exponent"), [&] {
                    calculator.enter_operation(Operation::POWER);
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("square-root"), [&] {
                    calculator.enter_operation(Operation::SQRT);
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("slash-forward"), [&] {
                    calculator.enter_operation(Operation::DIVIDE);
                }),

                Widget::button(Button::OUTLINE, "7", [&] {
                    calculator.enter_number(7);
                }),

                Widget::button(Button::OUTLINE, "8", [&] {
                    calculator.enter_number(8);
                }),

                Widget::button(Button::OUTLINE, "9", [&] {
                    calculator.enter_number(9);
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("close"), [&] {
                    calculator.enter_operation(Operation::MULTIPLY);
                }),

                Widget::button(Button::OUTLINE, "4", [&] {
                    calculator.enter_number(4);
                }),

                Widget::button(Button::OUTLINE, "5", [&] {
                    calculator.enter_number(5);
                }),

                Widget::button(Button::OUTLINE, "6", [&] {
                    calculator.enter_number(6);
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("minus"), [&] {
                    calculator.enter_operation(Operation::SUBSTRACT);
                }),

                Widget::button(Button::OUTLINE, "1", [&] {
                    calculator.enter_number(1);
                }),

                Widget::button(Button::OUTLINE, "2", [&] {
                    calculator.enter_number(2);
                }),

                Widget::button(Button::OUTLINE, "3", [&] {
                    calculator.enter_number(3);
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("plus"), [&] {
                    calculator.enter_operation(Operation::ADD);
                }),

                Widget::button(Button::OUTLINE, Graphic::Icon::get("plus-minus-variant")),

                Widget::button(Button::OUTLINE, "0", [&] {
                    calculator.enter_number(0);
                }),

                Widget::button(Button::OUTLINE, "."),

                Widget::button(Button::FILLED, Graphic::Icon::get("equal"), [&] {
                    calculator.equal();
                }),
            }
        )
    );

    auto win = Widget::window(
        Widget::vflow(
            0,
            {
                titlebar,
                screen_container,
                buttons_container
            }
        )
    );
    // clang-format on

    calculator.did_update();

    win->resize_to_content();
    win->show();

    return Application::run();
}
