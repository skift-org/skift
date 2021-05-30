#pragma once

#include <libwidget/Components.h>
#include <libwidget/Layouts.h>

#include "calculator/Engine.h"

namespace Calculator
{

auto keypad(Engine &calculator)
{
    using namespace Widget;

    // clang-format off

    return spacing(
        6,
        grid(
            4, 6, 4, 4,
            {
                outline_button(Graphic::Icon::get("percent")),

                outline_button("CE", [&] {
                    calculator.clear_all();
                }),

                outline_button("C", [&] {
                    calculator.clear();
                }),

                outline_button(Graphic::Icon::get("backspace-outline")),

                outline_button("1/x", [&] {
                    calculator.enter_operation(Operation::RECIPROCAL);
                }),

                outline_button(Graphic::Icon::get("exponent"), [&] {
                    calculator.enter_operation(Operation::POWER);
                }),

                outline_button(Graphic::Icon::get("square-root"), [&] {
                    calculator.enter_operation(Operation::SQRT);
                }),

                outline_button(Graphic::Icon::get("slash-forward"), [&] {
                    calculator.enter_operation(Operation::DIVIDE);
                }),

                outline_button("7", [&] {
                    calculator.enter_number(7);
                }),

                outline_button("8", [&] {
                    calculator.enter_number(8);
                }),

                outline_button("9", [&] {
                    calculator.enter_number(9);
                }),

                outline_button(Graphic::Icon::get("close"), [&] {
                    calculator.enter_operation(Operation::MULTIPLY);
                }),

                outline_button("4", [&] {
                    calculator.enter_number(4);
                }),

                outline_button("5", [&] {
                    calculator.enter_number(5);
                }),

                outline_button("6", [&] {
                    calculator.enter_number(6);
                }),

                outline_button(Graphic::Icon::get("minus"), [&] {
                    calculator.enter_operation(Operation::SUBSTRACT);
                }),

                outline_button("1", [&] {
                    calculator.enter_number(1);
                }),

                outline_button("2", [&] {
                    calculator.enter_number(2);
                }),

                outline_button("3", [&] {
                    calculator.enter_number(3);
                }),

                outline_button(Graphic::Icon::get("plus"), [&] {
                    calculator.enter_operation(Operation::ADD);
                }),

                outline_button(Graphic::Icon::get("plus-minus-variant")),

                outline_button("0", [&] {
                    calculator.enter_number(0);
                }),

                outline_button("."),

                filled_button(Graphic::Icon::get("equal"), [&] {
                    calculator.equal();
                }),
            }
        )
    );

    // clang-format on
}

} // namespace Calculator
